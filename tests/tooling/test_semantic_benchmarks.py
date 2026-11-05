"""Defend current semantic benchmark work counts, sampling and failure bounds."""
import json
import os
import sys
from pathlib import Path

import pytest

from scripts.objc3c_toolchain.semantic_benchmarks import (SCALES, expected_runtime,
    frontend_source, runtime_result, sample, summarize)
from scripts.objc3c_toolchain.semantic_runtime_oracles import CURRENT_WORKLOAD_VERSION


def calibration_receipt(path, *, offset=0.0, version=1):
    from scripts.objc3c_toolchain.semantic_benchmarks import workload_inventory, workload_names
    from scripts.objc3c_toolchain.semantic_runtime_oracles import FAMILIES, expected_work
    workloads = sorted(workload_names(version))
    samples, summaries = [], []
    for workload_index, workload in enumerate(workloads):
        scale = int(workload.rsplit('-', 1)[1])
        rows = []
        for iteration in range(4):
            value = 0.01*(workload_index+1)+offset+iteration*0.0001
            row = {'workload': workload, 'sdk_index': 0, 'warmup': iteration == 0,
                'exit_code': 0, 'timed_out': False, 'wall_seconds': value,
                'cpu_seconds': value/2, 'peak_process_rss_kib': 1024+workload_index}
            if workload.startswith('runtime-'):
                nanoseconds = int(value*1e8)
                family = workload.rsplit('-', 1)[0].removeprefix('runtime-')
                semantic = expected_runtime(scale) if family == 'runtime' else expected_work(family, scale)
                if family == 'shared-pool-tasks': semantic['worker_mask'] = 15
                if family == 'generic-array': semantic.update(payload_copies=3*16*scale, destructions=3*16*scale)
                row.update(work_seconds=nanoseconds/1e9,
                           semantic_result={**semantic, 'nanoseconds': nanoseconds})
                if semantic.get('callbacks', 0):
                    row['task_completions_per_second'] = semantic['callbacks']/row['work_seconds']
            rows.append(row);samples.append(row)
        measured = rows[1:]
        summary = {'sdk_index': 0, 'workload': workload, 'wall': summarize(measured)}
        if workload.startswith('runtime-'): summary['work'] = summarize(measured, 'work_seconds')
        if workload.startswith(('runtime-task-', 'runtime-worker-', 'runtime-shared-')):
            summary['task_completions_per_second'] = summarize(measured, 'task_completions_per_second')
        summaries.append(summary)
    report = {'schema': 'objc3c.semantic-benchmarks.v1', 'status': 'PASS', 'correctness': 'PASS',
        'workload_version': version, 'timing_decision': 'advisory-inconclusive-uncalibrated',
        'repetitions': 3, 'summaries': summaries, 'samples': samples,
        'delay_control': {'status': 'PASS', 'semantic_output_unchanged': True},
        'host': {'system': 'Linux-runner', 'kernel': 'kernel', 'cpu_model': 'cpu', 'cpu_quota': 'max',
            'cpuset': '0-3', 'memory_limit': 'max', 'affinity': [0, 1, 2, 3], 'container': True,
            'runtime_threads': 1, 'noise_limits': 'shared'},
        'sdks': [{'seal_sha256': 'a'*64, 'source_build_id': 'matched-source', 'instrumentation': {}}],
        'inputs': {'/checkout/scripts/objc3c_toolchain/semantic_benchmarks.py': 'b'*64,
            '/checkout/tests/benchmarks/collections.cpp': 'c'*64}}
    if version == CURRENT_WORKLOAD_VERSION:
        report['workloads'] = workload_inventory(CURRENT_WORKLOAD_VERSION)
        report['inputs'].update({'/checkout/tests/benchmarks/runtime_workloads.cpp': 'd'*64,
            '/checkout/tests/benchmarks/ownership_workloads.mm': 'f'*64,
            '/checkout/scripts/objc3c_toolchain/semantic_runtime_oracles.py': 'e'*64})
        report['runtime_delay_controls'] = [{'family': family, 'status': 'PASS',
            'semantic_output_unchanged': True, 'minimum_control_effect_seconds': 0.008,
            'median_effect_seconds': 0.016, 'samples_seconds': [0.016]*3} for family in FAMILIES]
    path.write_text(json.dumps(report))
    return report


@pytest.mark.parametrize('scale', SCALES)
def test_frontend_workload_scales_real_carrier_guard_match_and_cleanup_forms(scale):
    text = frontend_source(scale)
    assert text.count('Optional(int)') == scale
    assert text.count('guard let') == text.count('match(result)') == text.count('defer {') == scale
    assert text.count('int function_') == scale


@pytest.mark.parametrize('mutation', ['zero', 'partial', 'checksum', 'missing', 'bool', 'timer'])
def test_less_work_wrong_results_and_missing_samples_never_look_faster(mutation):
    result = {**expected_runtime(32), 'nanoseconds': 1000}
    if mutation == 'zero': result['completed_operations'] = 0
    elif mutation == 'partial': result['completed_operations'] -= 1
    elif mutation == 'checksum': result['checksum'] += 1
    elif mutation == 'missing': del result['checksum']
    elif mutation == 'bool': result['completed_operations'] = True
    elif mutation == 'timer': result['nanoseconds'] = 0
    with pytest.raises(ValueError, match='semantic output'):
        runtime_result(json.dumps(result), 32)
    with pytest.raises(ValueError, match='three retained'):
        summarize([{'wall_seconds': 1}]*2)


def test_runtime_oracle_is_independent_of_measured_duration():
    expected = expected_runtime(256)
    assert expected['checksum'] == 16*sum(key+17 for key in range(256))
    assert expected['completed_operations'] == 16*(2*256+3)
    assert runtime_result(json.dumps({**expected, 'nanoseconds': 1}), 256)['checksum'] == expected['checksum']


def test_per_command_measurement_keeps_status_logs_cpu_and_individual_rss(tmp_path):
    row = sample([sys.executable, '-c', 'print("work")'], cwd=tmp_path,
                 environment=dict(os.environ), log=tmp_path / 'sample.log')
    assert row['exit_code'] == 0 and not row['timed_out']
    assert row['wall_seconds'] > 0 and row['cpu_seconds'] > 0 and row['peak_process_rss_kib'] > 0
    assert (tmp_path / 'sample.log').read_text() == 'work\n'
    assert len(row['log_sha256']) == 64


def test_timeout_is_retained_as_failure_with_no_successful_timing_claim(tmp_path):
    row = sample([sys.executable, '-c', 'import time;time.sleep(30)'], cwd=tmp_path,
                 environment=dict(os.environ), log=tmp_path / 'timeout.log', timeout=0.05)
    assert row['timed_out'] and row['exit_code'] < 0


def test_interrupted_measurement_reaps_its_detached_process(tmp_path, monkeypatch):
    from scripts.objc3c_toolchain import semantic_benchmarks as owner
    children = []
    def interrupt(pid, flags):
        children.append(pid)
        raise KeyboardInterrupt('controlled interruption')
    monkeypatch.setattr(owner.os, 'wait4', interrupt)
    with pytest.raises(KeyboardInterrupt):
        sample([sys.executable, '-c', 'import time;time.sleep(30)'], cwd=tmp_path,
               environment=dict(os.environ), log=tmp_path / 'interrupted.log')
    assert len(children) == 1
    with pytest.raises(ProcessLookupError): os.kill(children[0], 0)


def test_distribution_reports_spread_without_an_invented_regression_budget():
    result = summarize([{'wall_seconds': value} for value in (1, 2, 3)])
    assert result == {'median': 2, 'minimum': 1, 'maximum': 3, 'median_absolute_deviation': 1, 'samples': 3}


@pytest.mark.parametrize('value', [float('nan'), float('inf'), float('-inf')])
def test_nonfinite_measurements_cannot_qualify(value):
    with pytest.raises(ValueError, match='sample'):
        summarize([{'wall_seconds': 1}, {'wall_seconds': value}, {'wall_seconds': 2}])


@pytest.mark.parametrize('mutate_sdk', [False, True])
@pytest.mark.parametrize('shared_compiler', [False, True])
@pytest.mark.parametrize('extended_runtime', [False, True])
@pytest.mark.parametrize('instrumented_objc', [False, True])
def test_complete_receipt_keeps_workload_annotations_and_rechecks_sdk(
        tmp_path, monkeypatch, mutate_sdk, shared_compiler, extended_runtime, instrumented_objc):
    from scripts.objc3c_toolchain import semantic_benchmarks as owner
    repo, sdk = tmp_path / 'repo', tmp_path / 'sdk'
    (repo / 'tests/benchmarks').mkdir(parents=True)
    (repo / 'tests/benchmarks/collections.cpp').write_text('retained workload fixture')
    (repo / 'tests/benchmarks/runtime_workloads.cpp').write_text('retained extended workload fixture')
    (repo / 'tests/benchmarks/ownership_workloads.mm').write_text('retained ownership workload fixture')
    (sdk / 'lib/clang/23').mkdir(parents=True)
    (sdk / 'lib/libobjc.so').touch()
    calls = []
    def verify(prefix):
        calls.append(prefix)
        return {'target': 'x86_64-unknown-linux-gnu', 'seal_sha256': ('b' if mutate_sdk and len(calls)>1 else 'a')*64,
                'source_build_id': 'test-only-compiler'}
    def command(argv, *, log, **configuration):
        if instrumented_objc and '--undefined-only' in argv and str(argv[-1]).endswith('/libobjc.so'):
            log.write_text('__asan_init\n')
            return {'wall_seconds': 0.001, 'cpu_seconds': 0.0005, 'peak_process_rss_kib': 1024,
                    'exit_code': 0, 'timed_out': False, 'log': str(log)}
        if Path(argv[0]).name == 'collections':
            nanos = 16_000_000 if str(argv[-1]) == '1' else 1000
            log.write_text(json.dumps({**expected_runtime(int(argv[1])), 'nanoseconds': nanos}))
        elif Path(argv[0]).name in ('arc-ownership', 'mrc-ownership'):
            from scripts.objc3c_toolchain.semantic_runtime_oracles import expected_work
            result = expected_work(Path(argv[0]).name, int(argv[1]))
            log.write_text(json.dumps({**result, 'nanoseconds': 16_000_000 if len(argv) == 5 else 1000}))
        elif Path(argv[0]).name == 'runtime_workloads':
            from scripts.objc3c_toolchain.semantic_runtime_oracles import expected_work
            family, scale = argv[1], int(argv[2])
            result = expected_work(family, scale)
            if family == 'shared-pool-tasks': result['worker_mask'] = 15
            if family == 'generic-array':
                result.update(payload_copies=3*16*scale, destructions=3*16*scale)
            log.write_text(json.dumps({**result, 'nanoseconds': 16_000_000 if len(argv) == 6 else 1000}))
        else:
            log.write_text('')
            if '-o' in argv:
                Path(argv[argv.index('-o')+1]).write_bytes(b'new artifact')
        return {'wall_seconds': 0.001, 'cpu_seconds': 0.0005, 'peak_process_rss_kib': 1024,
                'exit_code': 0, 'timed_out': False, 'log': str(log)}
    monkeypatch.setattr(owner, 'verify_sdk', verify)
    monkeypatch.setattr(owner, 'sample', command)
    monkeypatch.setattr(owner, 'host_context', lambda: {'cpu_quota': '4'})
    loader_inputs = []
    def loaded_graph(*args, **kwargs):
        loader_inputs.append(args[2])
        return {'libraries': {}}
    monkeypatch.setattr(owner, 'loaded_graph', loaded_graph)
    # Required paths are validated by the actual SDK owner in integration;
    # this subprocess-mocked test checks receipt completeness and final admission.
    if shared_compiler:
        for name in ('libLLVM.so', 'libclang-cpp.so'):
            (sdk / 'lib' / name).touch()
    output = repo / 'out/evidence'
    if instrumented_objc:
        with pytest.raises(ValueError, match='instrumented service archive or Objective-C runtime'):
            owner.measure(repo, output, sdk=sdk, extended_runtime=extended_runtime)
        assert json.loads((output / 'qualification.json').read_text())['status'] == 'FAIL'
        return
    if mutate_sdk:
        with pytest.raises(ValueError, match='SDK changed'):
            owner.measure(repo, output, sdk=sdk, extended_runtime=extended_runtime)
        assert json.loads((output / 'qualification.json').read_text())['status'] == 'FAIL'
    else:
        report = owner.measure(repo, output, sdk=sdk, extended_runtime=extended_runtime)
        assert report['status'] == 'PASS' and report['correctness'] == 'PASS'
        assert len(report['summaries']) == (33 if extended_runtime else 9)
        assert all(row['wall']['samples'] == 3 for row in report['summaries'])
        assert len([row for row in report['samples'] if row.get('semantic_result')]) == (108 if extended_runtime else 12)
        assert report['delay_control']['status'] == 'PASS'
        from scripts.objc3c_assurance.evidence import suite_results
        import copy
        import shutil
        native_root = repo / 'out/native'
        destination = native_root / 'semantic-workloads/measurement';destination.parent.mkdir(parents=True)
        shutil.move(output, destination)
        if not extended_runtime:
            with pytest.raises(ValueError): suite_results(native_root, 'semantic-workloads')
            return
        from test_semantic_import_benchmarks import orchestration_receipt
        imports_root = tmp_path / 'import-fixture';imports_root.mkdir()
        imports, _, _ = orchestration_receipt(imports_root, monkeypatch)
        imports_destination = destination.parent / 'imports'
        shutil.copytree(imports_root / 'repo/out/measurement', imports_destination)
        assert len(suite_results(native_root, 'semantic-workloads')) == 58
        for mutation in ('summary', 'sample', 'delay', 'oracle'):
            broken = copy.deepcopy(report)
            if mutation == 'summary': broken['summaries'].pop()
            elif mutation == 'sample':
                broken['samples'] = [row for row in broken['samples'] if row.get('workload') != 'syntax-32']
            elif mutation == 'delay': broken['delay_control']['status'] = 'FAIL'
            else:
                next(row for row in broken['samples'] if row.get('semantic_result'))['semantic_result']['completed_operations'] = 0
            (destination / 'qualification.json').write_text(json.dumps(broken))
            with pytest.raises(ValueError): suite_results(native_root, 'semantic-workloads')
    assert len(calls) == 2
    assert bool(loader_inputs[0]) is shared_compiler


@pytest.mark.parametrize('imports', [False, True])
def test_development_benchmark_prefix_keeps_exact_native_products_and_runtime_headers(tmp_path, imports):
    from scripts.objc3c_toolchain.semantic_benchmarks import prepare_prefix
    stage, prefix = tmp_path / 'stage', tmp_path / 'prefix'
    for relative in ('bin/clang', 'bin/clang++', 'bin/ld.lld', 'lib/libLLVM.so.23.1', 'lib/libclang-cpp.so.23.1',
        'lib/libobjc.so.4.6', 'lib/libobjc3_runtime_services.a', 'lib/clang/23/include/stddef.h',
        'include/objc3c/runtime/objc3_collections.h'):
        path = stage / relative;path.parent.mkdir(parents=True, exist_ok=True);path.write_text(relative)
    (stage / 'lib/libobjc.so').symlink_to('libobjc.so.4.6')
    if imports:
        (stage / 'bin/objc3c').write_text('facade')
        (stage / 'include/objc').mkdir()
        (stage / 'include/objc/runtime.h').write_text('objc headers')
    result = prepare_prefix(stage, prefix, source_build_id='current-build', target='x86_64-unknown-linux-gnu', imports=imports)
    assert (prefix / 'bin/objc3c').exists() is imports
    assert (prefix / 'share/objc3c/verify-sdk.py').exists() is imports
    assert result['source_build_id'] == 'current-build'
    assert (prefix / 'lib/libLLVM.so').resolve() == prefix / 'lib/libLLVM.so.23.1'
    assert (prefix / 'lib/libclang-cpp.so').resolve() == prefix / 'lib/libclang-cpp.so.23.1'
    assert (prefix / 'lib/libobjc.so').resolve() == prefix / 'lib/libobjc.so.4.6'
    assert (stage / 'bin/clang').stat().st_ino == (prefix / 'bin/clang').stat().st_ino
    with pytest.raises(ValueError, match='fresh independent'):
        prepare_prefix(stage, prefix, source_build_id='current-build', target='x86_64-unknown-linux-gnu')


def test_development_benchmark_prefix_accepts_a_static_compiler_stage(tmp_path):
    from scripts.objc3c_toolchain.semantic_benchmarks import prepare_prefix
    stage, prefix = tmp_path / 'stage', tmp_path / 'prefix'
    for relative in ('bin/clang', 'bin/clang++', 'bin/ld.lld', 'lib/libobjc.so.4.6',
        'lib/libobjc3_runtime_services.a', 'lib/clang/23/include/stddef.h',
        'include/objc3c/runtime/objc3_collections.h'):
        path = stage / relative;path.parent.mkdir(parents=True, exist_ok=True);path.write_text(relative)
    (stage / 'lib/libobjc.so').symlink_to('libobjc.so.4.6')
    result = prepare_prefix(stage, prefix, source_build_id='static-build', target='x86_64-unknown-linux-gnu')
    assert result['source_build_id'] == 'static-build'
    assert not (prefix / 'lib/libLLVM.so').exists()
    assert not (prefix / 'lib/libclang-cpp.so').exists()


@pytest.mark.parametrize('path', ['stdlib/include/objc3/ObjC3Map.h', 'native/objc3c/src/runtime/stdlib/collections_service.cpp',
    'tests/benchmarks/collections.cpp', 'scripts/objc3c_toolchain/semantic_benchmarks.py',
    'scripts/objc3c_toolchain/semantic_import_benchmarks.py', 'toolchains/llvm/patches/change.patch'])
def test_semantic_work_changes_select_the_existing_native_lane(path):
    from scripts.objc3c_assurance.plan import select
    plan = select([path], head='a'*40, base='b'*40, tested='c'*40)
    suite = next(row for row in plan['suites'] if row['id'] == 'semantic-workloads')
    assert 'semantic-work-count' in suite['execution'] and 'advisory-timing' in suite['execution']


@pytest.mark.parametrize('version', [1, CURRENT_WORKLOAD_VERSION])
def test_independent_hosted_receipts_calibrate_observed_variance_and_cost_only(tmp_path, version):
    from scripts.objc3c_toolchain.semantic_benchmark_calibration import calibrate
    repo = tmp_path / 'repo';(repo / 'out').mkdir(parents=True)
    first, second = tmp_path / 'first.json', tmp_path / 'second.json'
    calibration_receipt(first, version=version)
    calibration_receipt(second, offset=0.0002, version=version)
    report = calibrate(repo, [first, second], repo / 'out/calibration',
                       labels=['workflow-run-a', 'workflow-run-b'])
    assert report['status'] == 'PASS' and report['decision'] == 'calibrated-observation-advisory'
    assert report['independent_receipts'] == 2 and report['samples_per_workload'] == 6
    assert len(report['observations']) == (9 if version == 1 else 33)
    assert len(report['scaling']) == (3 if version == 1 else 11)
    assert all(row['pooled']['samples'] == 6 and row['decision'] == 'observed-runner-variance-only'
               for row in report['observations'])
    assert report['measured_cost']['commands'] == (72 if version == 1 else 264)
    assert 'do not define a product regression budget' in report['gate_policy']
    assert json.loads((repo / 'out/calibration/calibration.json').read_text()) == report


@pytest.mark.parametrize('mutation', ['duplicate', 'host', 'sdk', 'summary', 'oracle', 'warmup'])
def test_calibration_rejects_reused_or_incompatible_or_corrupt_receipts(tmp_path, mutation):
    from scripts.objc3c_toolchain.semantic_benchmark_calibration import calibrate
    repo = tmp_path / 'repo';(repo / 'out').mkdir(parents=True)
    first, second = tmp_path / 'first.json', tmp_path / 'second.json'
    calibration_receipt(first)
    report = calibration_receipt(second, offset=0.0002)
    if mutation == 'duplicate': second.write_text(json.dumps(json.loads(first.read_text()), indent=2))
    elif mutation == 'host': report['host']['cpu_model'] = 'different';second.write_text(json.dumps(report))
    elif mutation == 'sdk': report['sdks'][0]['seal_sha256'] = 'd'*64;second.write_text(json.dumps(report))
    elif mutation == 'summary': report['summaries'][0]['wall']['median'] += 1;second.write_text(json.dumps(report))
    elif mutation == 'oracle':
        row = next(row for row in report['samples'] if row['workload'] == 'runtime-32')
        row['semantic_result']['completed_operations'] = 0;second.write_text(json.dumps(report))
    elif mutation == 'warmup':
        next(row for row in report['samples'] if row['workload'] == 'syntax-32')['warmup'] = False
        second.write_text(json.dumps(report))
    with pytest.raises(ValueError):
        calibrate(repo, [first, second], repo / 'out/calibration')


@pytest.mark.parametrize('fault', ['workload-contract', 'delay-control', 'payload-lifetime', 'throughput'])
def test_extended_calibration_rechecks_the_expanded_contract_and_oracles(tmp_path, fault):
    from scripts.objc3c_toolchain.semantic_benchmark_calibration import calibrate
    repo = tmp_path / 'repo';(repo / 'out').mkdir(parents=True)
    first, second = tmp_path / 'first.json', tmp_path / 'second.json'
    calibration_receipt(first, version=CURRENT_WORKLOAD_VERSION)
    report = calibration_receipt(second, version=CURRENT_WORKLOAD_VERSION, offset=0.0002)
    if fault == 'workload-contract': report['workloads'][0]['timeout_seconds'] = 0
    elif fault == 'delay-control': report['runtime_delay_controls'].pop()
    elif fault == 'payload-lifetime':
        next(row for row in report['samples'] if row['workload'] == 'runtime-generic-array-32')[
            'semantic_result']['live_values'] = 1
    else:
        next(row for row in report['samples'] if row['workload'] == 'runtime-worker-tasks-32')[
            'task_completions_per_second'] *= 2
    second.write_text(json.dumps(report))
    with pytest.raises(ValueError): calibrate(repo, [first, second], repo / 'out/calibration')
