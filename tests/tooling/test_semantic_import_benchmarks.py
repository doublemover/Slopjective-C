"""Import phase accounting, independent provider semantics and cache controls."""
from copy import deepcopy
import json
import os
from pathlib import Path
import re
import shutil
import subprocess

import pytest

from scripts.objc3c_toolchain import semantic_import_benchmarks as owner


@pytest.mark.parametrize('scale', owner.SCALES)
def test_import_generator_exercises_each_carrier_provider_and_guarded_consumer(scale):
    bundle = owner.sources(scale)
    assert len(bundle['exports']) == scale
    assert bundle['header'].count('Optional(int)') == scale
    assert bundle['provider'].count('Some(int,') == scale
    assert bundle['body'].count('guard let') == scale
    assert owner.expected(scale) == {'checksum': sum(range(17, 17+scale)), 'completed_operations': scale}
    assert bundle == owner.sources(scale)


@pytest.mark.parametrize('fault', ['empty', 'partial', 'wrong-provider', 'bool', 'extra'])
def test_oracle_rejects_missing_work_wrong_provider_and_fast_early_exits(fault):
    result = owner.expected(32)
    if fault == 'empty': text = ''
    else:
        if fault == 'partial': result['completed_operations'] -= 1
        elif fault == 'wrong-provider': result['checksum'] += 32
        elif fault == 'bool': result['completed_operations'] = True
        else: result['unexpected'] = 0
        text = json.dumps(result)
    with pytest.raises(ValueError): owner.semantic_output(text, 32)


@pytest.mark.parametrize('stage', ['cold', 'unchanged', 'changed-leaf'])
def test_reuse_contract_rejects_skipped_builds_and_unnecessary_consumer_rebuilds(stage):
    reuse = {'cold': (False, False), 'unchanged': (True, True), 'changed-leaf': (False, True)}[stage]
    report = {'schema': 'objc3c.source-graph-build.v4', 'status': 'PASS',
        'nodes': {name: {'reused': value} for name, value in zip((owner.MODULE, '@application'), reuse)}}
    assert owner.validate_reuse(report, stage) == dict(zip((owner.MODULE, '@application'), reuse))
    for name in report['nodes']:
        broken = deepcopy(report);broken['nodes'][name]['reused'] = not broken['nodes'][name]['reused']
        with pytest.raises(ValueError): owner.validate_reuse(broken, stage)
    report['nodes'].pop('@application')
    with pytest.raises(ValueError): owner.validate_reuse(report, stage)


def test_real_separate_provider_and_pch_controls_reject_wrong_but_valid_native_code(tmp_path):
    """Standard-C overlap checks linkage/PCH mechanics, not ObjC3 qualification."""
    compiler = shutil.which('clang')
    if not compiler: pytest.skip('Clang unavailable for native import mechanism controls')
    bundle = owner.sources(32)
    # Adapt only the carriers/guard syntax to their scalar overlap. The compiler
    # fork is required for the unmodified workload and is not substituted here.
    def scalar(text):
        text = text.replace('Optional(int)', 'int')
        text = re.sub(r'Some\(int, (.*?)\);', r'(\1);', text)
        return re.sub(r'guard let (\w+) = (\w+) else \{ return 3; \}', r'int \1 = \2;', text)
    header, provider, consumer = tmp_path / 'bench.h', tmp_path / 'provider.c', tmp_path / 'consumer.c'
    header.write_text(scalar(bundle['header']));provider.write_text(scalar(bundle['provider']))
    pch, po = tmp_path / 'bench.pch', tmp_path / 'provider.o'
    def command(*args):
        return subprocess.run([compiler, '-std=c17', '-O2', *map(str, args)], check=True,
                              capture_output=True, text=True, timeout=30)
    command('-c', provider, '-o', po)
    command('-x', 'c-header', header, '-o', pch)
    for transport in ('source', 'pch'):
        consumer.write_text(('#include "bench.h"\n' if transport == 'source' else '')+scalar(bundle['body']))
        co, binary = tmp_path / (transport+'.o'), tmp_path / transport
        command(*(['-include-pch', pch] if transport == 'pch' else []), '-c', consumer, '-o', co)
        command(po, co, '-o', binary)
        result = subprocess.run([binary], check=True, capture_output=True, text=True, timeout=30)
        assert owner.semantic_output(result.stdout, 32) == owner.expected(32)
        provider.write_text(scalar(owner.sources(32, bias=1)['provider']))
        mutant = tmp_path / (transport+'-wrong.o');command('-c', provider, '-o', mutant)
        command(mutant, co, '-o', binary)
        result = subprocess.run([binary], capture_output=True, text=True, timeout=30)
        assert result.returncode == 4 and not result.stdout
        with pytest.raises(ValueError): owner.semantic_output(result.stdout, 32)


def fake_sdk(tmp_path):
    repo, sdk = tmp_path / 'repo', tmp_path / 'sdk'
    repo.mkdir()
    for name in ('bin/clang', 'bin/clang++', 'bin/objc3c', 'bin/ld.lld', 'lib/libobjc.so',
                 'lib/libobjc3_runtime_services.a'):
        path = sdk / name;path.parent.mkdir(parents=True, exist_ok=True);path.write_bytes(b'fixture')
    return repo, sdk


def orchestration_receipt(tmp_path, monkeypatch, fault=''):
    repo, sdk = fake_sdk(tmp_path)
    admissions, builds, commands = [], {}, []
    def verify(prefix):
        admissions.append(prefix)
        return {'target': 'x86_64-unknown-linux-gnu', 'source_build_id': 'fixture',
                'seal_sha256': ('b' if fault == 'changed-sdk' and len(admissions) > 1 else 'a')*64}
    def module():
        return {'canonical_name': owner.MODULE, 'package_identity': owner.PACKAGE,
                'semantic_fingerprint': 'c'*64, 'runtime_abi': 'gnustep-2.2;arc'}
    def sample(argv, *, log, **kwargs):
        argv = list(map(str, argv));commands.append(argv)
        log.write_text('')
        code = 0
        if 'scripts.objc3c_toolchain.source_graph' in argv:
            build = Path(argv[argv.index('--output-dir')+1]);build.mkdir(exist_ok=True)
            iteration = builds.setdefault(str(build), 0);builds[str(build)] += 1
            reused = [(False, False), (True, True), (False, True)][iteration]
            if fault == 'wrong-reuse': reused = (True, True)
            (build / 'build-report.json').write_text(json.dumps({
                'schema': 'objc3c.source-graph-build.v4', 'status': 'PASS',
                'sdk_seal': 'a'*64,
                'modules': {owner.MODULE: module()},
                'nodes': {name: {'reused': reuse} for name, reuse in zip((owner.MODULE, '@application'), reused)}}))
        elif argv[0].endswith('.bin'):
            if 'wrong-provider' in argv[0]: code = 4
            else: log.write_text(json.dumps({**owner.expected(32), **({'checksum': 0} if fault == 'wrong-work' else {})}))
        elif '-o' in argv or '--emit-object' in argv:
            option = '-o' if '-o' in argv else '--emit-object'
            output = Path(argv[argv.index(option)+1])
            if output.name == 'missing-provider.bin':
                code = 1;log.write_text('undefined symbol: bench_value_0')
            elif fault != 'missing-output': output.write_bytes(b'fresh object')
            if '--emit-interface' in argv:
                interface = Path(argv[argv.index('--emit-interface')+1]);interface.write_text('serialized interface')
                interface.with_suffix('.objc3i.json').write_text(json.dumps({'module': module()}))
            if '--emit-dependency-record' in argv:
                assert not (output.parent / 'producer-inputs').exists()
                record = Path(argv[argv.index('--emit-dependency-record')+1])
                record.write_text(json.dumps({'schema': 'objc3c.dependency-record.v2', 'modules': []
                    if fault == 'missing-import' else [{'fingerprint': module()['semantic_fingerprint'],
                        'package': owner.PACKAGE, 'runtime': module()['runtime_abi']}]}))
        return {'argv': argv, 'exit_code': code, 'timed_out': False, 'log': str(log),
                'log_sha256': owner.digest(log),
                'wall_seconds': .001, 'cpu_seconds': .0005, 'peak_process_rss_kib': 1024}
    monkeypatch.setattr(owner, 'verify_sdk', verify)
    monkeypatch.setattr(owner, 'sample', sample)
    monkeypatch.setattr(owner, 'host_context', lambda: {'host': 'fixture'})
    monkeypatch.setattr(owner, 'loaded_graph', lambda *args, **kwargs: {'libraries': {}})
    output = repo / 'out/measurement'
    return owner.measure(repo, output, sdk=sdk, source_graph=True), admissions, builds


@pytest.mark.parametrize('fault', ['', 'missing-output', 'missing-import', 'wrong-work', 'changed-sdk', 'wrong-reuse'])
def test_full_import_orchestration_checks_source_removal_all_phases_reuse_and_sdk(tmp_path, monkeypatch, fault):
    if fault:
        with pytest.raises(ValueError): orchestration_receipt(tmp_path, monkeypatch, fault)
        assert json.loads((tmp_path / 'repo/out/measurement/qualification.json').read_text())['status'] == 'FAIL'
    else:
        report, admissions, builds = orchestration_receipt(tmp_path, monkeypatch)
        assert report['status'] == 'PASS' and report['correctness'] == 'PASS'
        assert len(admissions) == 2 and len(builds) == 4
        assert len(report['summaries']) == 25
        assert all(row['wall']['samples'] == 3 for row in report['summaries'])
        imported = [row for row in report['samples'] if row.get('source_free')]
        assert len(imported) == 8
        assert len([row for row in report['samples'] if row.get('semantic_result')]) == 24
        assert len([row for row in report['samples'] if row.get('control')]) == 4
        assert report['timing_decision'] == 'advisory-inconclusive-uncalibrated'


@pytest.mark.parametrize('fault', ['stale-log', 'stale-product', 'missing-product', 'nonzero', 'timeout'])
def test_import_command_rejects_stale_success_missing_products_and_timeouts(tmp_path, monkeypatch, fault):
    repo, sdk = fake_sdk(tmp_path)
    output = repo / 'out';(output / 'logs').mkdir(parents=True)
    report = {'samples': []}
    measurement = owner.Measurement(repo, output, sdk, {'target': 'x86_64-unknown-linux-gnu'}, report)
    product = output / 'object.o'
    if fault == 'stale-log': (output / 'logs/action.log').write_text('old success')
    if fault == 'stale-product': product.write_bytes(b'old object')
    def sample(argv, *, log, **kwargs):
        log.write_text('diagnostic')
        return {'exit_code': 1 if fault == 'nonzero' else 0, 'timed_out': fault == 'timeout',
                'log': str(log), 'wall_seconds': .001}
    monkeypatch.setattr(owner, 'sample', sample)
    with pytest.raises(ValueError): measurement.run('action', ['unused-compiler'], outputs=[product])


@pytest.mark.parametrize('fault', ['phase', 'sample', 'duplicate', 'warmup', 'summary', 'cpu',
    'work', 'source-free', 'products', 'control', 'reuse', 'fingerprint', 'generator',
    'log', 'raw-output', 'snapshot', 'retained-source'])
def test_import_terminal_gate_recomputes_raw_evidence_and_rejects_false_success(tmp_path, monkeypatch, fault):
    report, _, _ = orchestration_receipt(tmp_path, monkeypatch)
    root = tmp_path / 'repo/out/measurement'
    assert len(owner.verify_report(report, root)) == 25
    if fault == 'phase': report['summaries'].pop()
    elif fault == 'sample': report['samples'].pop()
    elif fault == 'duplicate': report['samples'].append(report['samples'][0])
    elif fault == 'warmup': next(row for row in report['samples'] if 'phase' in row)['warmup'] = False
    elif fault == 'summary': report['summaries'][0]['wall']['median'] += .001
    elif fault == 'cpu': report['samples'][0]['cpu_seconds'] = float('nan')
    elif fault == 'work': next(row for row in report['samples'] if 'semantic_result' in row)['semantic_result']['checksum'] = 0
    elif fault == 'source-free': next(row for row in report['samples'] if 'source_free' in row)['source_free'] = False
    elif fault == 'products': next(row for row in report['samples'] if row.get('phase') == 'producer')['outputs'] = {}
    elif fault == 'control': report['samples'] = [row for row in report['samples'] if row.get('control') != 'missing-provider']
    elif fault == 'reuse': next(row for row in report['samples'] if row.get('phase') == 'unchanged')['reused'][owner.MODULE] = False
    elif fault == 'fingerprint': next(row for row in report['samples'] if row.get('phase') == 'changed-leaf')['semantic_fingerprint'] = 'd'*64
    elif fault == 'generator': report['workloads'][0]['generated_inputs_sha256'] = 'd'*64
    elif fault == 'log':
        log = Path(report['samples'][0]['log']);log.write_text('changed native diagnostic')
    elif fault == 'raw-output':
        row = next(row for row in report['samples'] if 'semantic_result' in row)
        log = Path(row['log']);log.write_text('{"checksum":0,"completed_operations":32}')
        row['log_sha256'] = owner.digest(log)
    elif fault == 'snapshot':
        row = next(row for row in report['samples'] if row.get('phase') == 'unchanged')
        snapshot = Path(row['build_report']['path'])
        build = json.loads(snapshot.read_text());build['nodes'][owner.MODULE]['reused'] = False
        snapshot.write_text(json.dumps(build));row['build_report']['sha256'] = owner.digest(snapshot)
    else: (root / 'workload-source-32.json').write_text('{}')
    with pytest.raises(ValueError): owner.verify_report(report, root)


def test_import_evidence_relocates_without_the_sdk_or_original_build_tree(tmp_path, monkeypatch):
    report, _, _ = orchestration_receipt(tmp_path, monkeypatch)
    original = tmp_path / 'repo/out/measurement'
    relocated = tmp_path / 'artifact/imports'
    # GitHub retains the same compact JSON/log subset as the native owner.
    from scripts.objc3c_assurance.evidence import native_files
    for source in native_files(original):
        destination = relocated / source.relative_to(original)
        destination.parent.mkdir(parents=True, exist_ok=True);shutil.copyfile(source, destination)
    shutil.rmtree(tmp_path / 'repo');shutil.rmtree(tmp_path / 'sdk')
    assert len(owner.verify_report(report, relocated)) == 25
