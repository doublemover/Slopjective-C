"""Defend current semantic benchmark work counts, sampling and failure bounds."""
import json
import os
import sys
from pathlib import Path

import pytest

from scripts.objc3c_toolchain.semantic_benchmarks import (SCALES, expected_runtime,
    frontend_source, runtime_result, sample, summarize)


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


def test_distribution_reports_spread_without_an_invented_regression_budget():
    result = summarize([{'wall_seconds': value} for value in (1, 2, 3)])
    assert result == {'median': 2, 'minimum': 1, 'maximum': 3, 'median_absolute_deviation': 1, 'samples': 3}


@pytest.mark.parametrize('mutate_sdk', [False, True])
def test_complete_receipt_keeps_workload_annotations_and_rechecks_sdk(tmp_path, monkeypatch, mutate_sdk):
    from scripts.objc3c_toolchain import semantic_benchmarks as owner
    repo, sdk = tmp_path / 'repo', tmp_path / 'sdk'
    (repo / 'tests/benchmarks').mkdir(parents=True)
    (repo / 'tests/benchmarks/collections.cpp').write_text('retained workload fixture')
    (sdk / 'lib/clang/23').mkdir(parents=True)
    calls = []
    def verify(prefix):
        calls.append(prefix)
        return {'target': 'x86_64-unknown-linux-gnu', 'seal_sha256': ('b' if mutate_sdk and len(calls)>1 else 'a')*64,
                'source_build_id': 'test-only-compiler'}
    def command(argv, *, log, **configuration):
        if Path(argv[0]).name == 'collections':
            nanos = 16_000_000 if str(argv[-1]) == '1' else 1000
            log.write_text(json.dumps({**expected_runtime(int(argv[1])), 'nanoseconds': nanos}))
        else:
            log.write_text('')
            if '-o' in argv:
                Path(argv[argv.index('-o')+1]).write_bytes(b'new artifact')
        return {'wall_seconds': 0.001, 'cpu_seconds': 0.0005, 'peak_process_rss_kib': 1024,
                'exit_code': 0, 'timed_out': False, 'log': str(log)}
    monkeypatch.setattr(owner, 'verify_sdk', verify)
    monkeypatch.setattr(owner, 'sample', command)
    monkeypatch.setattr(owner, 'host_context', lambda: {'cpu_quota': '4'})
    monkeypatch.setattr(owner, 'loaded_graph', lambda *args, **kwargs: {'libraries': {}})
    # Required paths are validated by the actual SDK owner in integration;
    # this subprocess-mocked test checks receipt completeness and final admission.
    for name in ('libLLVM.so', 'libclang-cpp.so'):
        (sdk / 'lib' / name).touch()
    output = repo / 'out/evidence'
    if mutate_sdk:
        with pytest.raises(ValueError, match='SDK changed'):
            owner.measure(repo, output, sdk=sdk)
        assert json.loads((output / 'qualification.json').read_text())['status'] == 'FAIL'
    else:
        report = owner.measure(repo, output, sdk=sdk)
        assert report['status'] == 'PASS' and report['correctness'] == 'PASS'
        assert len(report['summaries']) == 9
        assert all(row['wall']['samples'] == 3 for row in report['summaries'])
        assert len([row for row in report['samples'] if row.get('semantic_result')]) == 12
        assert report['delay_control']['status'] == 'PASS'
        from scripts.objc3c_assurance.evidence import suite_results
        import copy
        import shutil
        native_root = repo / 'out/native'
        destination = native_root / 'semantic-workloads/measurement';destination.parent.mkdir(parents=True)
        shutil.move(output, destination)
        assert len(suite_results(native_root, 'semantic-workloads')) == 9
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


def test_development_benchmark_prefix_keeps_exact_native_products_and_runtime_headers(tmp_path):
    from scripts.objc3c_toolchain.semantic_benchmarks import prepare_prefix
    stage, prefix = tmp_path / 'stage', tmp_path / 'prefix'
    for relative in ('bin/clang', 'bin/clang++', 'bin/ld.lld', 'lib/libLLVM.so.23.1', 'lib/libclang-cpp.so.23.1',
        'lib/libobjc.so.4.6', 'lib/libobjc3_runtime_services.a', 'lib/clang/23/include/stddef.h',
        'include/objc3c/runtime/objc3_collections.h'):
        path = stage / relative;path.parent.mkdir(parents=True, exist_ok=True);path.write_text(relative)
    for name, actual in (('libLLVM.so', 'libLLVM.so.23.1'), ('libclang-cpp.so', 'libclang-cpp.so.23.1'),
                         ('libobjc.so', 'libobjc.so.4.6')):
        (stage / 'lib' / name).symlink_to(actual)
    result = prepare_prefix(stage, prefix, source_build_id='current-build', target='x86_64-unknown-linux-gnu')
    assert result['source_build_id'] == 'current-build'
    assert (prefix / 'lib/libobjc.so').resolve() == prefix / 'lib/libobjc.so.4.6'
    assert (stage / 'bin/clang').stat().st_ino == (prefix / 'bin/clang').stat().st_ino
    with pytest.raises(ValueError, match='fresh independent'):
        prepare_prefix(stage, prefix, source_build_id='current-build', target='x86_64-unknown-linux-gnu')


@pytest.mark.parametrize('path', ['stdlib/include/objc3/ObjC3Map.h', 'native/objc3c/src/runtime/stdlib/collections_service.cpp',
    'tests/benchmarks/collections.cpp', 'scripts/objc3c_toolchain/semantic_benchmarks.py', 'toolchains/llvm/patches/change.patch'])
def test_semantic_work_changes_select_the_existing_native_lane(path):
    from scripts.objc3c_assurance.plan import select
    plan = select([path], head='a'*40, base='b'*40, tested='c'*40)
    suite = next(row for row in plan['suites'] if row['id'] == 'semantic-workloads')
    assert 'semantic-work-count' in suite['execution'] and 'advisory-timing' in suite['execution']
