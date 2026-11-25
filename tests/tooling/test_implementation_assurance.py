"""Exact selection, retained evidence and cleanup controls for implementation analysis."""
import json
import os
from pathlib import Path
import shutil
import subprocess
from zipfile import ZipFile

import pytest

from scripts.objc3c_implementation_assurance import evidence
from scripts.objc3c_implementation_assurance.plan import select, validate
from scripts.objc3c_toolchain.analysis_checkpoint import analysis_argv
from scripts.objc3c_toolchain.analysis_partition import FULL_PARTITIONS
from scripts.objc3c_toolchain.implementation_analysis import checker_configuration
from scripts.objc3c_toolchain.llvm_tools import identity

REV = dict(head='a'*40, base='b'*40, tested='c'*40)


def test_selection_is_scoped_dependency_aware_and_periodically_complete():
    assert not select(['README.md'], **REV)['required']
    selected = select(['native/objc3c/src/runtime/service.cpp', 'README.md'], **REV)
    assert selected['required'] and selected['analysis_paths'] == [
        'native/objc3c/src/runtime/service.cpp']
    assert selected['selection'] == 'dependency-aware-owned-changes'
    periodic = select(['README.md'], **REV, periodic=True)
    assert periodic['required'] and periodic['analysis_paths'] == []
    assert periodic['selection'] == 'complete-owned-tu-inventory'
    assert select([], **REV)['required'] and select(None, **REV)['required']


def test_manual_comparison_uses_the_tested_commit_and_exact_ancestor(tmp_path):
    from scripts.objc3c_implementation_assurance.__main__ import main

    def git(*args):
        return subprocess.check_output(['git', '-C', tmp_path, '-c', 'user.name=Fixture',
            '-c', 'user.email=fixture@example.test', *args], text=True).strip()

    git('init', '--quiet')
    git('commit', '--quiet', '--allow-empty', '-m', 'base')
    base = git('rev-parse', 'HEAD')
    header = tmp_path / 'native/objc3c/src/runtime/public/header.h'
    header.parent.mkdir(parents=True)
    header.write_text('#pragma once\n')
    git('add', '.')
    git('commit', '--quiet', '-m', 'native header')
    tested = git('rev-parse', 'HEAD')
    destination = tmp_path / 'out/plan.json'
    assert main(['plan', '--repo', str(tmp_path), '--plan', str(destination),
                 '--periodic-sha', tested, '--comparison-base', base]) == 0
    result = json.loads(destination.read_text())
    assert result['revision'] == {'head': tested, 'tested': tested, 'base': base}
    assert result['periodic'] is False and result['analysis_partitions'] == 1
    assert result['analysis_paths'] == ['native/objc3c/src/runtime/public/header.h']


def test_offline_replay_requires_reviewed_zip_bytes_and_owned_members(tmp_path):
    from scripts.objc3c_implementation_assurance.replay import reviewed_zip

    path = tmp_path / '1.zip'
    with ZipFile(path, 'w') as archive:
        archive.writestr('plan.json', '{}')
    with reviewed_zip(tmp_path, 1, evidence.digest(path)) as archive:
        assert archive.read('plan.json') == b'{}'
    with pytest.raises(ValueError, match='differs from the reviewed archive'):
        reviewed_zip(tmp_path, 1, '0'*64)
    with ZipFile(path, 'w') as archive:
        archive.writestr('../unowned', 'data')
    with pytest.raises(ValueError, match='unowned or excessive'):
        reviewed_zip(tmp_path, 1, evidence.digest(path))


@pytest.mark.parametrize('mutation', ['required', 'path', 'digest'])
def test_modified_analysis_plan_is_rejected(mutation):
    plan = select(['native/analysis/CMakeLists.txt'], **REV)
    if mutation == 'required': plan['required'] = False
    elif mutation == 'path': plan['analysis_paths'] = []
    else: plan['sha256'] = '0'*64
    with pytest.raises(ValueError):
        validate(plan)


def passing_bundle(tmp_path, *, periodic=False, partition=None):
    from scripts.objc3c_toolchain.analysis_partition import partition_spec, partition_rows
    tmp_path.mkdir(parents=True, exist_ok=True)
    if periodic and partition is None:
        for index in range(FULL_PARTITIONS):
            plan, _ = passing_bundle(tmp_path / ('part-'+str(index)), periodic=True,
                                     partition=partition_spec(index,FULL_PARTITIONS))
        return plan, None
    plan = select(['native/objc3c/src/runtime/service.cpp'], **REV, periodic=periodic)
    analysis = tmp_path / 'analysis';analysis.mkdir()
    # Producer paths intentionally do not exist on the receiving machine.
    producer_repo = Path('/producer/checkout')
    producer = producer_repo / 'out/implementation-assurance/results/analysis'
    config = producer / 'checks.json'
    tool = '/producer/tool/objc3-clang-tidy'
    policy = json.loads((Path(__file__).resolve().parents[2] /
                         'toolchains/llvm/implementation-analysis.json').read_text())
    (analysis / 'checks.json').write_text(json.dumps(checker_configuration(producer_repo,
        producer_repo / 'out/toolchains/objc3-development/src', policy)))
    inventory = []
    for index, owner in enumerate(('frontend', 'standalone-runtime', 'maintained-clang')):
        source = (producer_repo / 'native/objc3c/src/runtime/service.cpp' if index < 2 else
                  producer_repo / 'out/toolchains/objc3-development/src/clang/lib/Sema/Sema.cpp')
        inventory.append({'owner': owner, 'identity': str(index+1)*64, 'file': str(source),
            'directory': '/producer/build/'+owner, 'output': '/producer/build/'+owner+'/source.o',
            'analysis_arguments': ['clang++', '-DREAL_FLAG=1', '-c', str(source)],
            'dependencies': [str(source)], 'selected': periodic or index < 2,
            'dependency_origin': 'actual-ninja-compilation'})
    if partition is not None:
        selected = {row['identity'] for owner in {row['owner'] for row in inventory}
                    for row in partition_rows([row for row in inventory if row['owner'] == owner and row['selected']],
                                              partition, seconds=policy['partition_seconds'])}
        for row in inventory:
            row['selected'] = row['identity'] in selected
    results = [{'identity': row['identity'], 'status': 'PASS', 'diagnostics': [],
                'execution_reused': False}
               for row in inventory if row['selected']]
    controls = [
        {'name': 'control-corrected', 'status': 'PASS', 'exit_code': 0,
         'expected_exit_code': 0, 'timed_out': False},
        {'name': 'control-defective', 'status': 'PASS', 'exit_code': 1,
         'expected_exit_code': 1, 'timed_out': False},
    ]
    commands = list(controls)
    for row in inventory:
        if not periodic and row['owner'] != 'maintained-clang':
            key = row['owner']+'-fresh-headers-'+identity(row['output'])[:16]
            raw = analysis / (key+'.d')
            raw.write_text('implementation-analysis: '+row['file']+'\n')
            dependency_file = str(producer / raw.name)
            row.update(dependency_origin='fresh-original-compiler-preprocessing', dependency_scan=key)
            commands.append({'name': key, 'status': 'PASS', 'exit_code': 0, 'expected_exit_code': 0,
                'timed_out': False, 'cwd': row['directory'],
                'argv': ['clang++', row['file'], '-M', '-MF', dependency_file, '-MT', 'implementation-analysis'],
                'outputs': {dependency_file: evidence.digest(raw)}})
        if not row['selected']:
            continue
        name = row['owner']+'-'+row['identity'][:16]
        directory = analysis / name;directory.mkdir()
        (directory / 'compile_commands.json').write_text(json.dumps([{
            'file': row['file'], 'directory': row['directory'], 'arguments': row['analysis_arguments']}]))
        commands.append({'name': name, 'status': 'PASS', 'exit_code': 0,
            'expected_exit_code': 0, 'timed_out': False,
            'argv': analysis_argv(tool, producer / name, row['file'], config, '/producer/resources')})
    logs = analysis / 'logs';logs.mkdir()
    for command in commands:
        name = command['name']
        path = logs / (name+'.log')
        path.write_text('/producer/control.cpp:2:1: error: null access '
                        '[clang-analyzer-core.NullDereference,-warnings-as-errors]\n'
                        if name == 'control-defective' else '')
        command.update(log=str(producer / 'logs' / path.name), log_sha256=evidence.digest(path))
    inner = {'schema': 'objc3c.implementation-analysis.v2', 'status': 'PASS',
        'subject': 'native-and-maintained-clang-implementation', 'policy': policy,
        'source_build_id': 'fork-build', 'build_config_digest': 'config',
        'tool': {'path': tool, 'sha256': 'a'*64}, 'tool_graph': {'libraries': {'libclang.so': 'b'*64}},
        'tool_objects': [{'object': 'tool.o'}],
        'inputs': {str(config): evidence.digest(analysis / 'checks.json'),
                   **{path:'c'*64 for row in inventory for path in row['dependencies']}},
        'configuration': str(config), 'repository_output': str(producer_repo / 'out'),
        'resource_directory': '/producer/resources',
        'changed_paths': [str(producer_repo / path) for path in plan['analysis_paths']],
        'results': results, 'inventory': inventory, 'commands': commands,
        'exclusions': [{'owner': row['owner'], 'selected_variants': 1,
                        'analyzed_variants': int(row['selected'])} for row in inventory],
        'analyzed_variants': len(results), 'inventory_variants': len(inventory),
        'fresh_native_executions': len(results), 'verified_checkpoint_actions': 0}
    if partition is not None:inner['partition']=partition
    (analysis / 'qualification.json').write_text(json.dumps(inner))
    report = {'schema': evidence.SCHEMA, 'status': 'PASS', 'revision': plan['revision'],
        'plan_sha256': plan['sha256'], 'analysis_paths': plan['analysis_paths'],
        'temporary_builds_reclaimed': True, 'commands': [], 'evidence_root': str(producer.parent),
        'analyzed_variants': len(results), 'inventory_variants': len(inventory),
        'source_build_id': 'fork-build', 'build_config_digest': 'config'}
    if partition is not None:report['partition']=partition
    build_logs = tmp_path / 'build-logs';build_logs.mkdir()
    for name in ('frontend-configure', 'frontend-build', 'runtime-configure', 'runtime-build'):
        path = build_logs / (name+'.log');path.write_text('successful build')
        report['commands'].append({'name': name, 'exit_code': 0, 'timed_out': False,
            'log': str(producer.parent / 'build-logs' / path.name), 'log_sha256': evidence.digest(path),
            'argv': ['cmake', '-DOBJC3C_COMPILER_INSTRUMENTATION_PROFILE=compiler-asan-ubsan',
                     '-DOBJC3C_LINK_CLANG_CPP=ON']})
    foundation = tmp_path / 'foundation';foundation.mkdir()
    (foundation / 'foundation-0.json').write_text(json.dumps({'tests': [{'name': 'compiler', 'code': 'PASS'}]}))
    (foundation / 'foundation.json').write_text(json.dumps({'status': 'passed',
        'source_identity': 'fork-build', 'phases': [{'phase': phase, 'exit_code': 0,
            **({'test_results': '/producer/foundation-0.json'} if phase == 'test' else {})}
            for phase in ('configure', 'build', 'test', 'stage')]}))
    report['artifacts'] = [evidence.artifact(tmp_path, path)
                           for path in evidence.evidence_files(tmp_path)]
    (tmp_path / 'result.json').write_text(json.dumps(report))
    return plan, report


@pytest.mark.parametrize('periodic', [False, True])
def test_complete_analysis_bundle_is_admitted_after_relocation(tmp_path, periodic):
    plan, _ = passing_bundle(tmp_path, periodic=periodic)
    result = evidence.aggregate(plan, tmp_path,
        outcomes={'plan': 'success', 'analysis': 'success'}, tested=REV['tested'])
    assert result['status'] == 'PASS' and result['analyzed_variants'] == (3 if periodic else 2)


def test_partitioned_analysis_uses_one_evidence_budget_per_worker(tmp_path, monkeypatch):
    plan, _ = passing_bundle(tmp_path, periodic=True)
    sizes = []
    for directory in tmp_path.iterdir():
        receipt = json.loads((directory / 'result.json').read_text())
        sizes.append(sum(row['bytes'] for row in receipt['artifacts']))
    per_worker = max(sizes)
    assert sum(sizes) > per_worker
    monkeypatch.setattr(evidence, 'MAX_BYTES', per_worker)
    result = evidence.aggregate(plan, tmp_path,
        outcomes={'plan': 'success', 'analysis': 'success'}, tested=REV['tested'])
    assert result['artifact_bytes'] == sum(sizes) and result['partitions'] == 8


@pytest.mark.parametrize('fault', ['cleanup', 'artifact', 'control', 'finding', 'outcome', 'dependency'])
def test_incomplete_analysis_bundle_fails_closed(tmp_path, fault):
    plan, report = passing_bundle(tmp_path)
    inner_path = tmp_path / 'analysis/qualification.json'
    inner = json.loads(inner_path.read_text())
    outcomes = {'plan': 'success', 'analysis': 'success'}
    if fault == 'cleanup': report['temporary_builds_reclaimed'] = False
    elif fault == 'artifact': report['artifacts'][0]['sha256'] = '0'*64
    elif fault == 'control': inner['commands'][1]['exit_code'] = 0
    elif fault == 'finding': inner['results'][0]['diagnostics'] = [{'check': 'blocker'}]
    elif fault == 'dependency': next((tmp_path / 'analysis').glob('*.d')).write_text('edited dependency output')
    else: outcomes['analysis'] = 'failure'
    inner_path.write_text(json.dumps(inner))
    if fault in ('control', 'finding', 'dependency'):
        report['artifacts'] = [evidence.artifact(tmp_path, path)
                               for path in evidence.evidence_files(tmp_path)]
    (tmp_path / 'result.json').write_text(json.dumps(report))
    with pytest.raises(ValueError):
        evidence.aggregate(plan, tmp_path, outcomes=outcomes, tested=REV['tested'])


def test_workflow_has_periodic_full_gate_and_automatic_cleanup():
    root = Path(__file__).resolve().parents[2]
    workflow = (root / '.github/workflows/implementation-analysis.yml').read_text()
    assert 'schedule:' in workflow and 'workflow_dispatch:' in workflow
    assert 'workflow_call:' in workflow
    assert 'comparison_base:' in workflow and 'results/**/*.d' in workflow
    assert 'Linux implementation analysis gate' in workflow
    assert 'needs: [plan, foundation]' in workflow
    assert 'needs: [plan, foundation, analysis]' in workflow
    assert '--foundation-bundle out/implementation-assurance/foundation-input' in workflow
    assert '--foundation-archive-sha256 "$ANALYSIS_FOUNDATION_SHA256"' in workflow
    assert '--retain-builds' not in workflow
    release = (root / '.github/workflows/platform-host-evidence.yml').read_text()
    assert 'uses: ./.github/workflows/native-assurance.yml' in release
    assert 'build-sdk --fetch' not in release
    native = (root / '.github/workflows/native-assurance.yml').read_text()
    assert 'workflow_call:' in native
    assert 'uses: ./.github/workflows/implementation-analysis.yml' in native
    assert '--sanitizer-producer out/instrumented-foundation/producer' in native
    assert 'needs: asan-foundation-producer' in native
    assert 'IMPLEMENTATION_ANALYSIS_RESULT: ${{ needs.implementation-analysis.result }}' in native
    assert 'product-producer:' in native and 'Preserve qualified SDK archive' in native
    assert 'needs: [plan, product-producer]' in native
    installer = native.split('  archive-install:\n', 1)[1].split('  gate:\n', 1)[0]
    assert 'archive_install: true' in release
    assert 'needs: [plan, product-producer]' in installer
    assert 'name: objc3c-linux-sdk' in installer
    assert 'out/toolchains/sdk-cache/*.tar.zst' in native
    runner = (root / 'scripts/objc3c_implementation_assurance/runner.py').read_text()
    assert "PROFILE = 'compiler-asan-ubsan'" in runner
    assert 'with foundation_lock(repo)' in runner


@pytest.mark.parametrize('fault', ['', 'revision', 'compiler', 'build-report'])
def test_release_analysis_archives_matching_sanitizer_build_once(tmp_path, monkeypatch, fault):
    from scripts.objc3c_assurance import plan as native_selection
    from scripts.objc3c_implementation_assurance import plan as analysis_selection, shared_foundation
    from scripts.objc3c_toolchain.binary_packages import digest

    revision = dict.fromkeys(REV, REV['tested'])
    selected = analysis_selection.select([], **revision, periodic=True)
    native = native_selection.select([], **revision, periodic=True)
    monkeypatch.setattr(shared_foundation, 'from_checkout', lambda *args, **kwargs: selected)
    monkeypatch.setattr(shared_foundation, 'native_plan_from_checkout', lambda *args, **kwargs: native)
    monkeypatch.setattr(shared_foundation, '_build_foundation',
                        lambda *args, **kwargs: pytest.fail('release rebuilt the sanitizer compiler'))

    development = tmp_path / 'out/toolchains/objc3-development'
    stage, build = development / 'stage', development / 'build'
    native_producer = tmp_path / 'out/sanitizer'
    for directory in (stage, build, native_producer / 'foundation'):
        directory.mkdir(parents=True)
    staged_tool = stage / 'clang'
    shutil.copyfile('/bin/true', staged_tool)
    compiled_unit = build / 'selected-unit.o'
    compiled_unit.write_bytes(b'selected build input')
    report = build / 'foundation-build-report.json'
    report.write_text(json.dumps({'status': 'passed', 'source_identity': 'exact-build', 'phases': []}))
    shutil.copyfile(report, native_producer / 'foundation/foundation.json')
    receipt_path = native_producer / 'producer.json'
    portable = {'source_build_id': 'exact-build'}
    identity = {'source_build_id': 'exact-build', 'files': {'clang': digest(staged_tool)}}
    retained = {'schema': 'objc3c.instrumented-foundation-bundle.v1', 'status': 'PASS',
                'revision': revision, 'plan_sha256': native['sha256'], 'identity': identity,
                'foundation_identity': portable}
    if fault == 'revision':
        retained['revision'] = REV
    if fault == 'compiler':
        retained['identity'] = {**identity, 'files': {'clang': '0'*64}}
    if fault == 'build-report':
        report.write_text(report.read_text() + '\n')
    receipt_path.write_text(json.dumps(retained))
    monkeypatch.setattr(shared_foundation, '_portable_identity', lambda *args: portable)
    monkeypatch.setattr(shared_foundation, '_identity', lambda *args: identity)
    monkeypatch.setattr(shared_foundation, '_build_files', lambda *args: {compiled_unit})
    monkeypatch.setattr(shared_foundation, 'build_plans', lambda *args, **kwargs:
        {'configure': {'paths': {'build': str(build), 'stage': str(stage)}}})
    destination = tmp_path / 'out/analysis'
    if fault:
        with pytest.raises(ValueError, match='sanitizer producer'):
            shared_foundation.produce(tmp_path, selected, destination, sanitizer_producer=native_producer)
        assert not (destination / 'foundation.tar.zst').exists()
        return
    result = shared_foundation.produce(tmp_path, selected, destination, sanitizer_producer=native_producer)
    assert result['status'] == 'PASS' and result['identity'] == identity
    assert result['sanitizer_producer_sha256'] == digest(receipt_path)
    assert result['archive']['members'] == [str(path.relative_to(tmp_path)) for path in (compiled_unit, staged_tool)]
    with pytest.raises(ValueError, match='differs from the producer job output'):
        shared_foundation.restore(tmp_path, selected, destination, tmp_path / 'evidence',
                                  expected_archive_sha256='0'*64)


def test_shared_foundation_preserves_actual_ninja_dependencies(tmp_path):
    from scripts.objc3c_implementation_assurance.shared_foundation import (
        _archive, _archive_members, _tablegen,
    )

    ninja, compiler = shutil.which('ninja'), shutil.which('cc')
    if not all((ninja, compiler, shutil.which('zstd'))):
        pytest.skip('Ninja, a host C compiler and zstd are required for archive transport')
    build, stage = tmp_path / 'out/build', tmp_path / 'out/stage'
    build.mkdir(parents=True)
    stage.mkdir()
    source = tmp_path / 'identity.c'
    source.write_text('int identity(int value) { return value; }\n')
    (build / 'build.ninja').write_text(
        f'rule cc\n  command = {compiler} -MMD -MF $out.d -c $in -o $out\n'
        '  deps = gcc\n  depfile = $out.d\n'
        f'build identity.o: cc {source}\n')
    subprocess.run([ninja, '-C', build], check=True, capture_output=True)
    original = (build / 'identity.o').stat().st_mtime_ns
    tool_source = tmp_path / 'generator.c'
    tool_source.write_text('#include <stdio.h>\nint main(void) { puts("restored generator"); }\n')
    tool = build / 'bin/llvm-tblgen'
    tool.parent.mkdir()
    subprocess.run([compiler, tool_source, '-o', tool], check=True, capture_output=True)
    (stage / 'target').write_text('real retained payload')
    (stage / 'alias').symlink_to('target')
    working_directory = build / 'lib/IR'
    working_directory.mkdir(parents=True)
    (working_directory / 'unselected.o').write_text('excluded object')
    bundle = tmp_path / 'foundation.tar.zst'
    receipt = _archive(tmp_path, bundle, (stage, build), {
        build / 'build.ninja', build / '.ninja_deps', build / 'identity.o', tool,
        stage / 'target', stage / 'alias', working_directory,
    })
    assert _archive_members(bundle, ('out/build/', 'out/stage/')) == receipt['members']
    shutil.rmtree(build)
    shutil.rmtree(stage)
    subprocess.run(['tar', '-I', 'zstd', '-xf', bundle, '-C', tmp_path], check=True)
    assert (build / 'identity.o').stat().st_mtime_ns == original
    assert working_directory.is_dir() and not list(working_directory.iterdir())
    assert (stage / 'alias').read_text() == 'real retained payload'
    assert subprocess.check_output([_tablegen(build)], text=True) == 'restored generator\n'
    dependencies = subprocess.check_output([ninja, '-C', build, '-t', 'deps', 'identity.o'], text=True)
    assert '(VALID)' in dependencies and str(source) in dependencies


def test_shared_foundation_rejects_an_archive_link_outside_the_owned_roots(tmp_path):
    from scripts.objc3c_implementation_assurance.shared_foundation import _archive_members

    if not shutil.which('zstd'):
        pytest.skip('zstd is required for archive transport')
    stage = tmp_path / 'out/stage'
    stage.mkdir(parents=True)
    (stage / 'alias').symlink_to('../../unowned')
    archive = tmp_path / 'foundation.tar.zst'
    subprocess.run(['tar', '-I', 'zstd', '-cf', archive, '-C', tmp_path, 'out/stage/alias'], check=True)
    with pytest.raises(ValueError, match='unowned link'):
        _archive_members(archive, ('out/stage/',))


def test_shared_foundation_selects_real_configured_compiler_inputs(tmp_path):
    from scripts.objc3c_implementation_assurance.shared_foundation import _archive, _build_files
    from scripts.objc3c_toolchain.implementation_analysis import dependencies, dependency_query, inventory, textual_pch

    cmake, ninja = shutil.which('cmake'), shutil.which('ninja')
    compiler = shutil.which('clang++')
    if not all((cmake, ninja, compiler, shutil.which('zstd'))):
        pytest.skip('CMake, Ninja, Clang and zstd are required for graph transport')
    source, build = tmp_path / 'source', tmp_path / 'out/build'
    (source / 'clang/lib').mkdir(parents=True)
    (source / 'clang/lib/owned.cpp').write_text('#include "table.inc"\nint selected() { return value; }\n')
    (source / 'ordinary.cpp').write_text('int unselected() { return 0; }\n')
    (source / 'tablegen.cpp').write_text('int main() { return 0; }\n')
    (source / 'table.in').write_text('constexpr int value = 42;\n')
    (source / 'support.in').write_text('constexpr int owned_support = 7;\n')
    (source / 'CMakeLists.txt').write_text('''
cmake_minimum_required(VERSION 3.20)
project(shared-foundation-control LANGUAGES CXX)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
configure_file(table.in "${CMAKE_BINARY_DIR}/include/llvm/table.inc" COPYONLY)
configure_file(support.in "${CMAKE_BINARY_DIR}/support/generated.hpp" COPYONLY)
add_executable(llvm-tblgen tablegen.cpp)
add_library(LLVM SHARED clang/lib/owned.cpp ordinary.cpp)
target_include_directories(LLVM PRIVATE "${CMAKE_BINARY_DIR}/include/llvm")
target_precompile_headers(LLVM PRIVATE "${CMAKE_BINARY_DIR}/support/generated.hpp")
set_target_properties(LLVM PROPERTIES VERSION 23.1 SOVERSION 23)
add_library(clang-cpp SHARED ordinary.cpp)
target_precompile_headers(clang-cpp PRIVATE <utility>)
set_target_properties(clang-cpp PROPERTIES VERSION 23.1 SOVERSION 23)
''')
    subprocess.run([cmake, '-S', source, '-B', build, '-G', 'Ninja',
                    f'-DCMAKE_MAKE_PROGRAM={ninja}', f'-DCMAKE_CXX_COMPILER={compiler}',
                    '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON'],
                   check=True, capture_output=True)
    subprocess.run([cmake, '--build', build], check=True, capture_output=True)
    working_directory = build / 'lib/IR'
    working_directory.mkdir()
    database = build / 'compile_commands.json'
    commands = json.loads(database.read_text())
    pch_sources = {
        Path(row['directory']) / row['file'] for row in commands
        if Path(row['file']).name.startswith('cmake_pch.') and 'clang-cpp.dir' in row['file']
    }
    assert pch_sources and all(path.is_file() for path in pch_sources)
    for row in commands:
        if Path(row['file']).name == 'ordinary.cpp':
            row['directory'] = str(working_directory)
            row['file'] = os.path.relpath(source / 'ordinary.cpp', working_directory)
    database.write_text(json.dumps(commands))
    patches = tmp_path / 'toolchains/llvm/patches'
    patches.mkdir(parents=True)
    (patches / 'owned.patch').write_text('+++ b/clang/lib/owned.cpp\n')
    trace = build / 'foundation-configuration-trace.jsonl'
    trace.write_text('{}\n')
    (build / 'foundation-build-report.json').write_text('{}\n')
    configuration = {'paths': {'build': str(build), 'source': str(source)},
        'host_environment': {'ninja_path': ninja}, 'configuration_trace': str(trace)}
    selected = _build_files(tmp_path, configuration)
    tablegen = build / 'bin/llvm-tblgen'
    assert tablegen in selected
    assert build / 'include/llvm/table.inc' in selected
    generated_support = build / 'support/generated.hpp'
    assert generated_support in selected
    assert build / 'CMakeFiles/rules.ninja' in selected
    assert working_directory in selected
    assert any(path.name == 'owned.cpp.o' for path in selected)
    assert not any(path.name == 'ordinary.cpp.o' for path in selected)
    assert {path.name for path in selected if path.parent == build / 'lib' and not path.is_dir()} == {
        'libLLVM.so', 'libLLVM.so.23', 'libLLVM.so.23.1',
        'libclang-cpp.so', 'libclang-cpp.so.23', 'libclang-cpp.so.23.1',
    }
    units = {source / 'clang/lib/owned.cpp'}
    graph = subprocess.check_output([ninja, '-C', build, '-t', 'commands'], text=True)
    original = inventory(database, graph, units)
    _, pch_proof = textual_pch(original[0])
    assert len(pch_proof) == 1 and Path(pch_proof[0]['bootstrap_pch']) in selected
    bundle = tmp_path / 'foundation.tar.zst'
    _archive(tmp_path, bundle, (build,), selected)
    shutil.rmtree(build)
    subprocess.run(['tar', '-I', 'zstd', '-xf', bundle, '-C', tmp_path], check=True)
    assert not any(path.exists() for path in pch_sources)
    restored_graph = subprocess.check_output([ninja, '-C', build, '-t', 'commands'], text=True)
    assert inventory(database, restored_graph, units) == original
    assert textual_pch(original[0])[1] == pch_proof
    pch = pch_proof[0]['bootstrap_pch']
    closure = subprocess.check_output(dependency_query(ninja, build, [pch]), text=True)
    assert str(generated_support) in dependencies(closure, build=build, selected_outputs={pch})[pch]
    tablegen.unlink()
    with pytest.raises(ValueError, match='executable llvm-tblgen'):
        _build_files(tmp_path, configuration)


def test_failed_shared_foundation_blocks_terminal_analysis(tmp_path):
    plan = select([], **REV, periodic=True)
    with pytest.raises(ValueError, match='foundation producer'):
        evidence.aggregate(plan, tmp_path, outcomes={
            'plan': 'success', 'foundation': 'failure', 'analysis': 'skipped',
        }, tested=REV['tested'])


@pytest.mark.parametrize('fault', ['missing-action', 'extra-action', 'wrong-selection', 'wrong-argv',
    'wrong-database', 'hidden-finding', 'missing-log', 'wrong-log-hash', 'reused-action',
    'missing-control-diagnostic', 'missing-build', 'wrong-profile', 'wrong-foundation', 'symlink',
    'wrong-resource-bound'])
def test_rehashed_outer_bundle_cannot_hide_missing_native_proof(tmp_path, fault):
    plan, report = passing_bundle(tmp_path)
    path = tmp_path / 'analysis/qualification.json'
    inner = json.loads(path.read_text())
    if fault == 'missing-action':
        inner['results'].pop();inner['analyzed_variants'] -= 1;inner['fresh_native_executions'] -= 1
        inner['exclusions'][1]['analyzed_variants'] = 0
    elif fault == 'extra-action':
        inner['results'].append({'identity': inner['inventory'][2]['identity'], 'status': 'PASS'})
        inner['analyzed_variants'] += 1;inner['fresh_native_executions'] += 1
        inner['exclusions'][2]['analyzed_variants'] = 1
    elif fault == 'wrong-selection': inner['inventory'][1]['selected'] = False
    elif fault == 'wrong-argv': inner['commands'][-1]['argv'].append('--checks=-*')
    elif fault == 'wrong-database':
        next((tmp_path / 'analysis').glob('frontend-*/compile_commands.json')).write_text('[]')
    elif fault == 'wrong-resource-bound':
        inner['inventory'][0]['analysis_timeout_seconds'] = 3600
        inner['results'][0]['timeout_seconds'] = 3600
    elif fault == 'hidden-finding':
        command = inner['commands'][-1]
        log = tmp_path / 'analysis/logs' / Path(command['log']).name
        log.write_text('/source/service.cpp:9:2: warning: moved value [bugprone-use-after-move]\n')
        command['log_sha256'] = evidence.digest(log)
    elif fault == 'missing-log':
        (tmp_path / 'analysis/logs' / Path(inner['commands'][-1]['log']).name).unlink()
    elif fault == 'wrong-log-hash': inner['commands'][-1]['log_sha256'] = '0'*64
    elif fault == 'reused-action':
        inner['fresh_native_executions'] -= 1;inner['verified_checkpoint_actions'] += 1
    elif fault == 'missing-control-diagnostic':
        log = tmp_path / 'analysis/logs/control-defective.log';log.write_text('some unrelated failure')
        inner['commands'][1]['log_sha256'] = evidence.digest(log)
    elif fault == 'missing-build': report['commands'].pop()
    elif fault == 'wrong-profile': report['commands'][0]['argv'] = ['cmake', '-S', 'frontend']
    elif fault == 'wrong-foundation':
        foundation = tmp_path / 'foundation/foundation.json'
        value = json.loads(foundation.read_text());value['source_identity'] = 'wrong-fork'
        foundation.write_text(json.dumps(value))
    else:
        log = tmp_path / 'analysis/logs/control-defective.log'
        alternate = tmp_path / 'original-control.txt';alternate.write_bytes(log.read_bytes())
        log.unlink();log.symlink_to(alternate)
    path.write_text(json.dumps(inner))
    report['analyzed_variants'] = inner['analyzed_variants']
    if fault != 'symlink':
        report['artifacts'] = [evidence.artifact(tmp_path, value)
                               for value in evidence.evidence_files(tmp_path)]
    (tmp_path / 'result.json').write_text(json.dumps(report))
    with pytest.raises(ValueError):
        evidence.aggregate(plan, tmp_path, outcomes={'plan': 'success', 'analysis': 'success'},
                           tested=REV['tested'])


@pytest.fixture
def lifecycle(tmp_path, monkeypatch):
    from scripts.objc3c_implementation_assurance import runner
    from scripts.objc3c_implementation_assurance.plan import from_checkout
    from scripts.objc3c_toolchain.build import foundation_lock
    from scripts.objc3c_toolchain.core import ToolchainError
    repo = tmp_path / 'repo';repo.mkdir()
    def git(*args):
        return subprocess.check_output(['git', '-C', str(repo), *args], text=True).strip()
    git('init', '-q');git('config', 'user.name', 'Analysis Test')
    git('config', 'user.email', 'analysis@example.invalid')
    (repo / '.gitignore').write_text('/out/\n')
    source = repo / 'native.cpp';source.write_text('int original;\n')
    git('add', '.');git('commit', '-qm', 'initial implementation')
    sha = git('rev-parse', 'HEAD')
    plan = from_checkout(repo, head=sha, base=sha, tested=sha)
    base = repo / 'out/toolchains/objc3-development'
    bootstrap = tmp_path / 'bootstrap';bootstrap.mkdir()
    build = base / ('build-linux-x86_64-'+runner.PROFILE)
    stage = base / ('stage-linux-x86_64-'+runner.PROFILE)
    def foundation(*args, **kwargs):
        with pytest.raises(ToolchainError, match='already holds'):
            with foundation_lock(repo):
                pytest.fail('runner released its shared build lock')
        for root in (base / 'src', build, stage): root.mkdir(parents=True)
        log = build / 'compile.log';log.write_text('native build diagnostic')
        (build / 'foundation-build-report.json').write_text(json.dumps({
            'status': 'passed', 'phases': [{'phase': 'build', 'log': str(log)}]}))
        return {'status': 'passed'}
    configuration = {'paths': {'build': str(build), 'stage': str(stage), 'source': str(base / 'src')},
        'host_environment': {'bootstrap_root': str(bootstrap), 'cmake_path': '/tools/cmake',
            'ninja_path': '/tools/ninja', 'resource_limits': {'compile_jobs': 2}}}
    monkeypatch.setattr(runner, '_build_foundation', foundation)
    monkeypatch.setattr(runner, 'build_plans', lambda *a, **kw: {'configure': configuration})
    def command(argv, *, log, **kwargs):
        if '-B' in argv: Path(argv[argv.index('-B')+1]).mkdir(parents=True, exist_ok=True)
        log.write_text('compiled successfully')
        return {'argv': list(map(str, argv)), 'exit_code': 0, 'timed_out': False,
                'log': str(log), 'log_sha256': evidence.digest(log)}
    monkeypatch.setattr(runner, 'execute_command', command)
    monkeypatch.setattr(runner, 'qualify', lambda *a, **kw: {
        'status': 'PASS', 'inventory_variants': 1, 'source_build_id': 'fork', 'build_config_digest': 'config'})
    monkeypatch.setattr(runner, 'analysis_results', lambda output: [{'status': 'PASS'}])
    return runner, repo, plan, base, source


def test_analysis_runner_holds_the_lock_and_cleans_only_its_created_roots(lifecycle):
    runner, repo, plan, base, _ = lifecycle
    output = repo / 'out/evidence'
    result = runner.run(repo, plan, output)
    assert result['status'] == 'PASS' and result['temporary_builds_reclaimed']
    assert (output / 'foundation/phase-0.log').read_text() == 'native build diagnostic'
    assert not (base / 'src').exists() and not (repo / 'out/implementation-assurance-builds').exists()
    assert '-DOBJC3C_COMPILER_INSTRUMENTATION_PROFILE=compiler-asan-ubsan' in result['commands'][0]['argv']


@pytest.mark.parametrize('fault', ['busy', 'preexisting', 'linked-parent', 'linked-output', 'dirty'])
def test_analysis_runner_preserves_unowned_work_before_start(lifecycle, tmp_path, fault):
    runner, repo, plan, base, source = lifecycle
    output = repo / 'out/evidence'
    protected = base / 'src';protected.mkdir(parents=True)
    marker = protected / 'do-not-delete';marker.write_text('existing development work')
    if fault == 'busy':
        from scripts.objc3c_toolchain.build import foundation_lock
        from scripts.objc3c_toolchain.core import ToolchainError
        with foundation_lock(repo), pytest.raises(ToolchainError, match='already holds'):
            runner.run(repo, plan, output)
    else:
        if fault == 'linked-parent':
            (repo / 'out/implementation-assurance-builds').symlink_to(tmp_path, target_is_directory=True)
        elif fault == 'linked-output': output.symlink_to(tmp_path, target_is_directory=True)
        elif fault == 'dirty': source.write_text('int changed;\n')
        with pytest.raises(ValueError): runner.run(repo, plan, output)
    assert marker.read_text() == 'existing development work'
    if fault == 'linked-output':
        assert not (output / 'result.json').exists()
    else:
        report = json.loads((output / 'result.json').read_text())
        assert report['status'] == 'FAIL'
        assert report['temporary_builds_reclaimed'] is True


@pytest.mark.parametrize('fault', ['foundation-failure', 'analysis-failure', 'source-mutated', 'diagnostic-failure'])
def test_partial_attempt_always_retains_failure_and_reclaims_created_builds(lifecycle, monkeypatch, fault):
    runner, repo, plan, base, source = lifecycle
    original = runner._build_foundation
    if fault in ('foundation-failure', 'diagnostic-failure'):
        def foundation(*args, **kwargs):
            original(*args, **kwargs)
            if fault == 'diagnostic-failure':
                path = base / ('build-linux-x86_64-'+runner.PROFILE) / 'foundation-build-report.json'
                path.write_text('malformed diagnostic receipt')
            raise ValueError('forced build failure')
        monkeypatch.setattr(runner, '_build_foundation', foundation)
    else:
        def qualify(*args, **kwargs):
            if fault == 'analysis-failure': raise ValueError('forced analysis failure')
            source.write_text('int mutated_while_analyzing;\n')
            return {'status': 'PASS', 'inventory_variants': 1,
                    'source_build_id': 'fork', 'build_config_digest': 'config'}
        monkeypatch.setattr(runner, 'qualify', qualify)
    output = repo / 'out/evidence'
    with pytest.raises(ValueError): runner.run(repo, plan, output)
    report = json.loads((output / 'result.json').read_text())
    assert report['status'] == 'FAIL' and report['temporary_builds_reclaimed']
    assert not (base / 'src').exists() and not (repo / 'out/implementation-assurance-builds').exists()
    if fault == 'diagnostic-failure': assert 'diagnostic_error' in report
