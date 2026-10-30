"""Adversarial receipts and process controls for the native LLVM assurance lane."""
from __future__ import annotations

import copy
import json
import os
from pathlib import Path
import struct
import sys

import pytest

from scripts.objc3c_toolchain.llvm_ir import corpus, instruction_body, module_facts, pipeline_options
from scripts.objc3c_toolchain.llvm_ir_execution import Execution, StageFailure, classify
from scripts.objc3c_toolchain.llvm_ir_inventory import stages, thinlto_modules, verify as verify_inventory
from scripts.objc3c_toolchain.llvm_reduce import interesting, matches, validate
from scripts.objc3c_toolchain.llvm_tools import verify_installation
from scripts.objc3c_toolchain.sanitizer_policy import strict_environment

ROOT = Path(__file__).resolve().parents[2]


def module(*, optnone=False):
    return ('target triple = "x86_64-unknown-linux-gnu"\n'
            'target datalayout = "e-m:e-p:64:64"\n'
            'define i32 @main() #0 {\n  ret i32 0, !dbg !4\n}\n'
            'attributes #0 = { '+('optnone noinline' if optnone else 'nounwind')+' }\n')


def test_corpus_keeps_authored_oracles_and_lowering_transports():
    document = corpus(ROOT)
    assert len(document['cases']) == 8
    assert {row['transport'] for row in document['cases']} == {'source', 'pch', 'interface'}
    assert sum(row.get('thinlto', False) for row in document['cases']) == 3
    assert {'arc', 'mrc'} == {row['ownership'] for row in document['cases']}
    assert {'c17', 'c++20', 'c++23'} == {row['standard'] for row in document['cases']}


def test_module_facts_reject_an_optimizer_with_no_work():
    assert module_facts(module(), 'x86_64-unknown-linux-gnu', require_optimizable=True)['optimizable_definitions'] == 1
    with pytest.raises(ValueError, match='all-optnone'):
        module_facts(module(optnone=True), 'x86_64-unknown-linux-gnu', require_optimizable=True)
    assert module_facts(module(optnone=True), 'x86_64-unknown-linux-gnu')['definitions'] == 1
    with pytest.raises(ValueError, match='declared target'):
        module_facts(module(), 'aarch64-unknown-linux-gnu')
    assert instruction_body(module()) == instruction_body(module().replace('!4', '!999'))
    assert instruction_body(module()) != instruction_body(module().replace('ret i32 0', 'ret i32 7'))


def test_arc_uses_clangs_exact_extension_points_and_size_uses_supported_pipeline():
    assert pipeline_options('O0', 'arc') == ['-passes=default<O0>', '-verify-each']
    assert pipeline_options('O2', 'mrc') == ['-passes=default<O2>', '-verify-each']
    expected = ['-passes=default<O2>', '-verify-each',
                '-passes-ep-pipeline-start=function(objc-arc-expand)',
                '-passes-ep-scalar-optimizer-late=objc-arc']
    assert pipeline_options('O2', 'arc') == expected
    assert pipeline_options('Os', 'arc') == expected
    with pytest.raises(ValueError):
        pipeline_options('experimental', 'arc')


def test_llvm_cannot_silently_ignore_a_bad_extension(tmp_path):
    runner, _ = execution(tmp_path)
    with pytest.raises(StageFailure, match='ignored an invalid pipeline extension'):
        runner.run('ignored-pass', [sys.executable, '-c',
            'print("Could not parse -passes-ep-pipeline-start pipeline: unknown pass")'])


def execution(tmp_path):
    report = {'status': 'running'}
    return Execution(tmp_path, {'PATH': os.defpath, 'LC_ALL': 'C'}, report, timeout=2), report


def test_missing_outputs_and_stale_outputs_fail_closed(tmp_path):
    runner, report = execution(tmp_path)
    with pytest.raises(StageFailure, match='missing or empty'):
        runner.run('missing', [sys.executable, '-c', 'pass'], outputs=[tmp_path / 'absent.o'])
    assert report['commands'][0]['exit_code'] == 0
    assert report['commands'][0].get('status') != 'PASS'
    stale = tmp_path / 'stale.o'
    stale.write_bytes(b'old object')
    with pytest.raises(StageFailure, match='stale output'):
        runner.run('stale', [sys.executable, '-c', 'pass'], outputs=[stale])
    assert stale.read_bytes() == b'old object'


def test_crashes_timeouts_unknown_passes_and_missing_tools_are_not_success(tmp_path):
    runner, report = execution(tmp_path)
    with pytest.raises(StageFailure) as timeout:
        runner.run('timeout', [sys.executable, '-c', 'import time; time.sleep(10)'], timeout=.1)
    assert timeout.value.classification == 'timeout'
    assert report['commands'][-1]['timed_out']
    with pytest.raises(StageFailure) as missing:
        runner.run('absent-tool', [tmp_path / 'no-opt'])
    assert missing.value.classification == 'infrastructure'
    with pytest.raises(StageFailure) as crash:
        runner.run('crash', [sys.executable, '-c', 'import os,signal; os.kill(os.getpid(),signal.SIGKILL)'],
                   stage='frontend-lowering')
    assert crash.value.classification == 'frontend-crash'
    assert classify('backend-verifier', -6, 'Bad machine code: bad PHI') == 'backend-verifier'
    assert classify('pass-verifier', -6, 'Broken module found') == 'pass-verifier'
    assert classify('input-verifier', 1, 'input module is broken') == 'invalid-ir'


def elf(runpath: str, capacity: int) -> bytes:
    """Small ELF64 with dynamic/string sections, plus observable code payload."""
    data = bytearray(512)
    data[:6] = b'\x7fELF\x02\x01'
    struct.pack_into('<Q', data, 40, 64)
    struct.pack_into('<HH', data, 58, 64, 3)
    struct.pack_into('<IIQQQQIIQQ', data, 128, 0, 6, 0, 0, 256, 32, 2, 0, 8, 16)
    struct.pack_into('<IIQQQQIIQQ', data, 192, 0, 3, 0, 0, 288, capacity, 0, 0, 1, 0)
    struct.pack_into('<QQ', data, 256, 29, 0)
    data[288:288+len(runpath)] = runpath.encode()
    data[-16:] = b'code-must-match!!'
    return bytes(data)


def test_install_identity_allows_only_exact_cmake_runpath_transition(tmp_path):
    expected = '$ORIGIN/../lib:$ORIGIN/../lib/x86_64-unknown-linux-gnu'
    before = expected+':'+str(tmp_path / 'build/lib')+':'
    canonical, installed = tmp_path / 'build.elf', tmp_path / 'installed.elf'
    canonical.write_bytes(elf(before, 160))
    installed.write_bytes(elf(expected, 160))
    assert verify_installation(canonical, installed, tmp_path / 'build')['status'] == 'PASS'
    data = bytearray(installed.read_bytes())
    data[-1] ^= 1
    installed.write_bytes(data)
    with pytest.raises(ValueError, match='outside its exact'):
        verify_installation(canonical, installed, tmp_path / 'build')
    installed.write_bytes(elf('/tmp/unowned', 160))
    with pytest.raises(ValueError, match='RUNPATH transition'):
        verify_installation(canonical, installed, tmp_path / 'build')


def spec():
    env = strict_environment(Path(sys.executable), Path('/usr/bin/false'))
    return {'schema': 'objc3c.llvm-interestingness.v1', 'timeout_seconds': 1,
            'tools': {sys.executable: 'unused'}, 'inputs': {}, 'environment': env,
            'commands': [{'name': 'specific-failure', 'argv': [sys.executable, '-c',
                'print("intended verifier signature"); raise SystemExit(3)']}],
            'failure': {'kind': 'diagnostic', 'stage': 'specific-failure', 'exit_code': 3,
                        'contains': ['intended verifier signature']}}


def test_reducer_rejects_unrelated_failures_crashes_and_timeouts():
    failure = spec()['failure']
    assert matches(failure, {'exit_code': 3, 'timed_out': False}, 'intended verifier signature')
    for result, text in [({'exit_code': 1, 'timed_out': False}, 'intended verifier signature'),
                         ({'exit_code': 3, 'timed_out': True}, 'intended verifier signature'),
                         ({'exit_code': 3, 'timed_out': False}, 'unrelated failure'),
                         ({'exit_code': 3, 'timed_out': False}, 'intended verifier signature\nerror: expected token')]:
        assert not matches(failure, result, text)


def test_interestingness_executes_bound_signature_and_rejects_tool_substitution(tmp_path):
    from scripts.objc3c_toolchain.binary_packages import digest
    document = spec()
    document['tools'][sys.executable] = digest(Path(sys.executable))
    candidate = tmp_path / 'input.ll'
    candidate.write_text('original retained')
    assert interesting(document, candidate, logs=tmp_path / 'logs')
    altered = copy.deepcopy(document)
    altered['tools'][sys.executable] = '0'*64
    with pytest.raises(ValueError, match='tool or dependency changed'):
        interesting(altered, candidate)
    altered = copy.deepcopy(document)
    altered['commands'][0]['argv'][-1] = 'print("unrelated failure"); raise SystemExit(3)'
    assert not interesting(altered, candidate)


@pytest.mark.parametrize('mutation', ('any-exit', 'no-signature', 'unbounded', 'shell', 'weak-policy', 'escaped-output'))
def test_reducer_rejects_ambiguous_specifications(mutation):
    document = spec()
    if mutation == 'any-exit':
        document['failure']['exit_code'] = 0
    elif mutation == 'no-signature':
        document['failure']['contains'] = []
    elif mutation == 'unbounded':
        document['timeout_seconds'] = 0
    elif mutation == 'shell':
        document['commands'][0]['argv'][0] = '/bin/sh'
    elif mutation == 'weak-policy':
        document['environment']['ASAN_OPTIONS'] = 'detect_leaks=0'
    elif mutation == 'escaped-output':
        document['commands'][0]['outputs'] = ['{work}/../stolen.ll']
    with pytest.raises(ValueError):
        validate(document)


def test_expensive_pilot_reports_measured_resource_constraints_without_qualification():
    from scripts.objc3c_toolchain.llvm_expensive_checks import resource_decision
    constrained = resource_decision(measured_build=100, measured_stage=50, free=199, factor=1.2, reserve=20)
    assert constrained['status'] == 'resource-constrained'
    assert constrained['estimated_required_bytes'] == 200
    assert resource_decision(measured_build=100, measured_stage=50, free=200,
                             factor=1.2, reserve=20)['status'] == 'admitted'
    with pytest.raises(ValueError):
        resource_decision(measured_build=-1, measured_stage=50, free=200, factor=1.2, reserve=20)


@pytest.mark.parametrize('mutation', ('none', 'missing-optimizer', 'wrong-route', 'missing-unit', 'failed-unit', 'duplicate-stage'))
def test_expected_inventory_rejects_missing_execution_even_with_many_commands(mutation):
    case = {'name': 'guard', 'transport': 'source', 'sources': ['main.mm']}
    commands = [{'name': name, 'status': 'PASS'} for name in stages(case, 'O2')]
    row = {'units': [{'source': 'main.mm', 'status': 'PASS'}],
           'executions': [{'route': route, 'status': 'PASS'} for route in ('production', 'split')]}
    if mutation == 'missing-optimizer':
        commands = [command for command in commands if command['name'] != 'guard-O2-0-optimizer']
        commands.extend({'name': 'unrelated-'+str(index), 'status': 'PASS'} for index in range(100))
    elif mutation == 'wrong-route':
        row['executions'][1]['route'] = 'production'
    elif mutation == 'missing-unit':
        row['units'] = []
    elif mutation == 'failed-unit':
        row['units'][0]['status'] = 'running'
    elif mutation == 'duplicate-stage':
        commands.append(commands[0])
    if mutation == 'none':
        assert verify_inventory(case, 'O2', row, commands)['status'] == 'PASS'
    else:
        with pytest.raises(ValueError):
            verify_inventory(case, 'O2', row, commands)


def test_thinlto_requires_real_per_module_postlink_optimization(tmp_path):
    # Combined preopt/index IR is a real LLD output, but does not establish
    # that an individual ThinLTO backend imported or optimized its module.
    source = tmp_path / 'unit/thin.o'
    source.parent.mkdir()
    source.write_bytes(b'bitcode input')
    for name in ('program.0.0.preopt.bc', 'program.index.bc'):
        (tmp_path / name).write_bytes(b'combined/link index')
    with pytest.raises(ValueError, match='per-module preopt'):
        thinlto_modules(tmp_path, [source])
    for index, stage in enumerate(('preopt', 'import', 'opt', 'precodegen')):
        (source.parent / (source.name+'.'+str(index)+'.'+stage+'.bc')).write_bytes(b'backend stage')
    assert len(thinlto_modules(tmp_path, [source])) == 5
    (source.parent / (source.name+'.2.opt.bc')).unlink()
    with pytest.raises(ValueError, match='per-module opt'):
        thinlto_modules(tmp_path, [source])


@pytest.mark.parametrize('change', ('runner-only', 'link-only', 'source', 'bootstrap', 'flags', 'tablegen'))
def test_debugger_reuse_cannot_cross_a_compilation_boundary(tmp_path, change):
    from scripts.objc3c_toolchain.llvm_debugger import adopt
    old = {'identity': 'old-receipt', 'build': str(tmp_path / 'old'),
           'binding': {'owner_sha256': 'old-owner', 'foundation_id': 'fork-1', 'bootstrap': 'bootstrap-1'},
           'configure': ['cmake', '-B', str(tmp_path / 'old'), '-DLLVM_USE_SANITIZER=Address;Undefined'],
           'tablegen_sha256': 'tablegen-1', 'targets': ['lldb', 'lldb-server']}
    new = copy.deepcopy(old)
    new.update(identity='new-receipt', build=str(tmp_path / 'new'))
    new['configure'][2] = new['build']
    new['binding']['owner_sha256'] = 'new-owner'
    if change == 'link-only':
        old['configure'].append('-DCMAKE_EXE_LINKER_FLAGS=-old')
        new['configure'].append('-DCMAKE_EXE_LINKER_FLAGS=-shared-libasan')
    elif change == 'source':
        new['binding']['foundation_id'] = 'fork-2'
    elif change == 'bootstrap':
        new['binding']['bootstrap'] = 'bootstrap-2'
    elif change == 'flags':
        new['configure'][-1] = '-DLLVM_USE_SANITIZER='
    elif change == 'tablegen':
        new['tablegen_sha256'] = 'tablegen-2'
    if change in ('runner-only', 'link-only'):
        reused = adopt(new, tmp_path / 'old', old)
        if change == 'runner-only':
            assert reused['configure'] == old['configure']
        else:
            assert reused['configure'][-1] == '-DCMAKE_EXE_LINKER_FLAGS=-shared-libasan'
        assert reused['identity'] == new['identity']
        assert reused['reused_build_identity'] == old['identity']
    else:
        with pytest.raises(ValueError, match='compilation recipe'):
            adopt(new, tmp_path / 'old', old)
