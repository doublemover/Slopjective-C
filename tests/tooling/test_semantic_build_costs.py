"""Real CMake/install timing controls and immutable operational receipt readers."""
from copy import deepcopy
import hashlib
import json
from pathlib import Path
import shutil
import subprocess

import pytest

from scripts.objc3c_toolchain import build, semantic_build_costs as owner


@pytest.fixture(scope='module')
def native_build_reports(tmp_path_factory):
    root = tmp_path_factory.mktemp('native-operational-cost-controls')
    repo, source, directory, prefix = root / 'repo', root / 'source', root / 'build', root / 'prefix'
    (repo / 'out/toolchains/objc3-development/src').mkdir(parents=True)
    source.mkdir()
    (source / 'CMakeLists.txt').write_text('cmake_minimum_required(VERSION 3.24)\n'
        'project(operational_control C)\nset(LLVM_ENABLE_ASSERTIONS ON CACHE BOOL "")\n'
        'enable_testing()\nadd_executable(witness main.c)\nadd_test(NAME witness COMMAND witness)\n'
        'install(TARGETS witness RUNTIME DESTINATION bin)\n')
    main = source / 'main.c';main.write_text('int main(void) { return 0; }\n')
    compiler = shutil.which('clang') or shutil.which('cc')
    def plans(_repo, **kwargs):
        identity = hashlib.sha256(main.read_bytes()).hexdigest()
        commands = {'configure': ['cmake', '-S', source, '-B', directory, '-G', 'Ninja',
                '-DCMAKE_C_COMPILER='+compiler, '-DCMAKE_INSTALL_PREFIX='+str(prefix)],
            'build': ['cmake', '--build', directory, '--target', 'witness', '--parallel', '1'],
            'test': ['ctest', '--test-dir', directory, '--no-tests=error', '--output-on-failure'],
            'stage': ['cmake', '--install', directory]}
        return {phase: {'paths': {'build': str(directory)},
            'identity': phase+'-'+identity+('-staged' if prefix.exists() else '-unstaged'),
            'sdk_consumers': {'installed': {'present': prefix.exists()}},
            'source_build_id': identity, 'source_tree': identity, 'build_config_digest': 'control-config',
            'effective_configuration': {'LLVM_ENABLE_ASSERTIONS': 'ON'},
            'host_environment': {'compiler': compiler, 'ninja_path': shutil.which('ninja')},
            'command': list(map(str, argv))}
            for phase, argv in commands.items()}
    patch = pytest.MonkeyPatch();patch.setattr(build, 'build_plans', plans)
    reports = []
    try:
        # This is a native executor/measurement control, not an LLVM build or
        # proof of Objective-C 3 performance. Every CTest executes a real binary.
        for stage in ('cold', 'unchanged-a', 'unchanged-b', 'changed-source'):
            if stage == 'changed-source': main.write_text('int main(void) { return 6*7 == 42 ? 0 : 1; }\n')
            report = build.build_foundation(repo, host='linux-x86_64', stage=True)
            assert report['configured_build_targets']['required_targets'] == ['witness']
            reports.append(Path(report['run_directory']) / 'report.json')
        yield root, reports
    finally:
        patch.undo();shutil.rmtree(root)


def copy_report(source, root):
    root.mkdir(parents=True)
    report = json.loads(source.read_text())
    for row in report['phases']:
        shutil.copyfile(Path(row['log']), root / Path(row['log']).name)
    path = root / 'report.json';path.write_text(json.dumps(report))
    return path, report


def test_real_cold_incremental_and_invalidation_builds_keep_cache_and_input_contexts_distinct(native_build_reports, tmp_path):
    _, reports = native_build_reports
    records = [owner.foundation_record(path) for path in reports]
    assert [row['state'] for row in records] == ['cold-build-directory', 'incremental-exact-inputs',
                                                'incremental-exact-inputs', 'invalidation-different-inputs']
    assert records[0]['context_sha256'] == records[1]['context_sha256'] == records[2]['context_sha256']
    assert records[3]['context_sha256'] != records[0]['context_sha256']
    assert all(set(row['phases_seconds']) == {'configure', 'build', 'test', 'stage'} for row in records)
    report = owner.collect(tmp_path, tmp_path / 'out/costs', foundations=reports)
    assert report['decision'] == 'advisory-inconclusive-uncalibrated' and len(report['groups']) == 3
    warm = next(row for row in report['groups'] if row['state'] == 'incremental-exact-inputs')
    assert warm['observations'] == warm['phases']['build']['samples'] == 2
    assert all(row['decision'] == 'observations-only-no-performance-pass' for row in report['groups'])


@pytest.mark.parametrize('fault', ['status', 'phase', 'missing-command', 'log', 'duration', 'interval',
    'cold-lie', 'reuse-identity', 'missing-prior', 'reuse-config', 'wrong-input-invalidation'])
def test_operational_build_reader_rejects_incomplete_stale_or_falsely_labeled_receipts(native_build_reports, tmp_path, fault):
    _, originals = native_build_reports
    path, report = copy_report(originals[1], tmp_path / 'record')
    if fault == 'status': report['status'] = 'failed'
    elif fault == 'phase': report['phases'][0]['exit_code'] = 1
    elif fault == 'missing-command': report['phases'][0]['command'] = []
    elif fault == 'log': (path.parent / Path(report['phases'][0]['log']).name).write_text('changed log')
    elif fault == 'duration': report['phases'][0]['elapsed_seconds'] = float('nan')
    elif fault == 'interval': report['elapsed_seconds'] = 0
    elif fault == 'cold-lie': report['cost_context']['initial_state'] = 'cold-build-directory'
    elif fault == 'reuse-identity': report['cost_context']['prior_source_identity'] = 'foreign source'
    elif fault == 'missing-prior': report['cost_context']['prior_report_sha256'] = None
    elif fault == 'reuse-config': report['cost_context']['prior_configuration_input_identity'] = 'f'*64
    else: report['cost_context']['initial_state'] = 'invalidation-different-inputs'
    path.write_text(json.dumps(report))
    with pytest.raises(ValueError): owner.foundation_record(path)


def test_cold_build_output_publication_does_not_invalidate_unchanged_inputs(native_build_reports):
    _, reports = native_build_reports
    cold, warm = [json.loads(path.read_text()) for path in reports[:2]]
    assert cold['phases'][0]['plan_identity'] != warm['phases'][0]['plan_identity']
    assert cold['cost_context']['configuration_input_identity'] == warm['cost_context']['configuration_input_identity']
    assert warm['cost_context']['initial_state'] == 'incremental-exact-inputs'


def test_configuration_changes_invalidate_even_when_source_is_identical(tmp_path):
    directory = tmp_path / 'build';directory.mkdir()
    for name in ('CMakeCache.txt', 'build.ninja'): (directory / name).write_text('configured')
    before = {'identity': 'old-plan', 'paths': {'build': str(directory)}, 'source_build_id': 'source',
              'build_config_digest': 'old-config', 'host_environment': {'compiler': 'admitted'}}
    context = build.cost_context(before, preparation_seconds=0, prepared=False)
    prior = {'status': 'passed', 'source_identity': 'source', 'cost_context': context,
             'phases': [{'phase': 'configure', 'plan_identity': 'old-plan'}]}
    (directory / 'foundation-build-report.json').write_text(json.dumps(prior))
    after = {**before, 'build_config_digest': 'changed-config'}
    assert build.cost_context(after, preparation_seconds=0, prepared=False)['initial_state'] == 'invalidation-different-inputs'


def test_duplicate_reformatted_build_report_cannot_create_an_independent_measurement(native_build_reports, tmp_path):
    _, reports = native_build_reports
    copy, value = copy_report(reports[0], tmp_path / 'duplicate')
    copy.write_text(json.dumps(value, indent=4))
    with pytest.raises(ValueError, match='independent'):
        owner.collect(tmp_path, tmp_path / 'out/duplicate', foundations=[reports[0], copy])


def test_foundation_costs_relocate_with_only_native_artifact_logs(native_build_reports, tmp_path):
    _, reports = native_build_reports
    value = json.loads(reports[0].read_text())
    artifact = tmp_path / 'artifact';artifact.mkdir()
    for index, row in enumerate(value['phases']):
        shutil.copyfile(Path(row['log']), artifact / ('phase-'+str(index)+'.log'))
    path = artifact / 'foundation.json';path.write_text(json.dumps(value))
    result = owner.foundation_record(path)
    assert result['state'] == 'cold-build-directory' and len(result['commands']) == 4
    (artifact / 'phase-0.log').unlink()
    (artifact / 'phase-0.log').symlink_to(Path(value['phases'][0]['log']))
    with pytest.raises(ValueError, match='log changed'): owner.foundation_record(path)


@pytest.fixture
def installation_receipt(tmp_path):
    if not shutil.which('zstd'): pytest.skip('native installer needs zstd')
    from test_toolchain_install import release
    from scripts.objc3c_toolchain.install_sdk import install
    archive, signature = release(tmp_path)
    first = install(archive, signature, tmp_path / 'installed')
    second = install(archive, signature, tmp_path / 'installed')
    first_path, second_path = tmp_path / 'first.json', tmp_path / 'second.json'
    first_path.write_text(json.dumps(first));second_path.write_text(json.dumps(second))
    return first_path, second_path, first, second


def test_real_archive_admission_and_existing_prefix_readmission_have_distinct_phase_costs(installation_receipt, tmp_path):
    first, second, before, after = installation_receipt
    assert before['seal_sha256'] == after['seal_sha256']
    assert owner.installation_record(first)['state'] == 'new-prefix-installed'
    assert owner.installation_record(second)['state'] == 'existing-prefix-readmitted'
    assert owner.installation_record(first)['context_sha256'] == owner.installation_record(second)['context_sha256']
    assert all(row['cost']['phases_seconds']['extraction'] > 0 for row in (before, after))
    report = owner.collect(tmp_path, tmp_path / 'out/install-costs', installations=[first, second])
    assert len(report['groups']) == 2 and all(row['decision'] == 'observations-only-no-performance-pass' for row in report['groups'])


def test_large_sdk_release_projects_to_a_bounded_exact_installation_receipt(tmp_path):
    cost = {'schema': 'objc3c.sdk-install-cost.v1', 'archive_sha256': 'a' * 64,
            'archive_bytes': 4096, 'source_build_id': 'current-source',
            'seal_sha256': 'b' * 64,
            'host': {'system': 'Linux', 'decoder': {'sha256': 'c' * 64}},
            'prefix_reused': False,
            'phases_seconds': {name: .1 for name in owner.INSTALL_PHASES},
            'elapsed_seconds': 1.0}
    installed = {'status': 'PASS', 'activated': True, 'seal_sha256': cost['seal_sha256'],
                 'cleanup_warnings': [], 'cost': cost}
    release = {'status': 'PASS', 'archive_sha256': cost['archive_sha256'],
               'archive_bytes': cost['archive_bytes'],
               'source_build_id': cost['source_build_id'],
               'seal_sha256': cost['seal_sha256'], 'installed': installed,
               'complete_qualification_history': 'x' * (9 * 1024 * 1024)}
    receipt = owner.bounded_installation_receipt(release)
    path = tmp_path / 'installation-cost.json'
    path.write_text(json.dumps(receipt))
    assert path.stat().st_size < 8 * 1024**2
    assert owner.installation_record(path)['context']['archive_sha256'] == 'a' * 64
    release['archive_sha256'] = 'f' * 64
    with pytest.raises(ValueError, match='identities differ'):
        owner.bounded_installation_receipt(release)


@pytest.mark.parametrize('fault', ['inactive', 'missing-phase', 'bad-duration', 'wrong-seal', 'wrong-archive', 'false-reuse', 'interval'])
def test_installation_costs_require_complete_archive_bound_actual_intervals(installation_receipt, fault):
    path, _, report, _ = installation_receipt
    broken = deepcopy(report)
    if fault == 'inactive': broken['activated'] = False
    elif fault == 'missing-phase': broken['cost']['phases_seconds'].pop('extraction')
    elif fault == 'bad-duration': broken['cost']['phases_seconds']['extraction'] = float('inf')
    elif fault == 'wrong-seal': broken['cost']['seal_sha256'] = 'f'*64
    elif fault == 'wrong-archive': broken['archive_sha256'] = 'f'*64
    elif fault == 'false-reuse': broken['cost']['prefix_reused'] = 1
    else: broken['cost']['elapsed_seconds'] = 0
    path.write_text(json.dumps(broken))
    with pytest.raises(ValueError): owner.installation_record(path)


def test_controlled_verifier_delay_is_attributed_to_admission_without_changing_the_sdk(tmp_path, monkeypatch):
    if not shutil.which('zstd'): pytest.skip('native installer needs zstd')
    import time
    from test_toolchain_install import release
    from scripts.objc3c_toolchain import install_sdk
    archive, signature = release(tmp_path)
    verify = install_sdk.verify
    def delayed(*args, **kwargs):
        time.sleep(.005)
        return verify(*args, **kwargs)
    monkeypatch.setattr(install_sdk, 'verify', delayed)
    result = install_sdk.install(archive, signature, tmp_path / 'installed')
    assert result['cost']['phases_seconds']['seal_verification_and_publication'] >= .005
    assert result['seal_sha256'] == result['cost']['seal_sha256'] and result['activated']
