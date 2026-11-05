"""Fault controls for revision selection, real native execution and terminal gates."""
from copy import deepcopy
import json
import os
from pathlib import Path
import subprocess
import shutil
import zipfile

import pytest

from scripts.objc3c_assurance import evidence
from scripts.objc3c_assurance.execution import checked_command, execute_ctest
from scripts.objc3c_assurance.plan import from_checkout, select, validate
from scripts.objc3c_assurance.runner import package_execution

REV = dict(head='a'*40, base='b'*40, tested='c'*40)


def test_native_package_uses_the_sdk_pipeline_install_without_requalifying_the_archive():
    result = {'status': 'PASS', 'archive_sha256': 'a' * 64,
              'installed': {'prefix': '/content-addressed/sdk'},
              'installed_qualification': {'status': 'PASS', 'commands': [
                  {'name': 'compile', 'exit_code': 0}, {'name': 'execute', 'exit_code': 0}]}}
    assert package_execution(result) == [{
        'status': 'PASS', 'archive_sha256': 'a' * 64,
        'installed_prefix': '/content-addressed/sdk', 'installed_consumer_commands': 2}]
    source = (Path(__file__).resolve().parents[2] /
              'scripts/objc3c_assurance/runner.py').read_text()
    assert "command('archive-install'" not in source


def test_package_terminal_inventory_matches_producer_after_artifact_relocation(tmp_path):
    result = {'status': 'PASS', 'archive_sha256': 'a'*64,
        'installed': {'prefix': '/original/runner/content-addressed/sdk'},
        'installed_qualification': {'status': 'PASS', 'commands': [
            {'name': 'compile', 'exit_code': 0}, {'name': 'execute', 'exit_code': 0}]}}
    producer = tmp_path / 'producer/package';producer.mkdir(parents=True)
    (producer / 'release.json').write_text(json.dumps(result))
    archive = tmp_path / 'package.zip'
    with zipfile.ZipFile(archive, 'w') as packed:
        packed.write(producer / 'release.json', 'package/release.json')
    shutil.rmtree(producer.parent)
    destination = tmp_path / 'different-gate-checkout'
    with zipfile.ZipFile(archive) as packed: packed.extractall(destination)
    assert evidence.suite_results(destination, 'package') == package_execution(result)


@pytest.mark.parametrize('fault', ['status', 'digest', 'malformed-digest', 'install', 'relative-prefix',
    'qualification', 'commands', 'command', 'timeout', 'boolean-exit'])
def test_native_package_rejects_incomplete_sdk_pipeline_evidence(fault):
    result = {'status': 'PASS', 'archive_sha256': 'a' * 64,
              'installed': {'prefix': '/content-addressed/sdk'},
              'installed_qualification': {'status': 'PASS', 'commands': [
                  {'name': 'execute', 'exit_code': 0}]}}
    if fault == 'status': result['status'] = 'FAIL'
    elif fault == 'digest': result['archive_sha256'] = ''
    elif fault == 'malformed-digest': result['archive_sha256'] = 'not-an-archive-digest'
    elif fault == 'install': result['installed'] = {}
    elif fault == 'relative-prefix': result['installed']['prefix'] = 'unowned/sdk'
    elif fault == 'qualification': result['installed_qualification']['status'] = 'FAIL'
    elif fault == 'commands': result['installed_qualification']['commands'] = []
    elif fault == 'command': result['installed_qualification']['commands'][0]['exit_code'] = 1
    elif fault == 'timeout': result['installed_qualification']['commands'][0]['timed_out'] = True
    else: result['installed_qualification']['commands'][0]['exit_code'] = False
    with pytest.raises(ValueError, match='installed archive qualification'):
        package_execution(result)


def test_failed_sdk_attempt_preserves_nested_native_logs_without_sdk_payloads(tmp_path):
    from scripts.objc3c_assurance.package_evidence import preserve, snapshot
    repo = tmp_path / 'repo'
    repo.mkdir()
    before = snapshot(repo)
    candidate = repo / 'out/toolchains/linux-sdk-builds/current'
    candidate.mkdir(parents=True)
    run = repo / 'out/native-validation/current-run'
    run.mkdir(parents=True)
    (run / 'qualification.json').write_text('{"status":"FAIL"}')
    (run / 'lookup.log').write_text('actual preprocessing diagnostic')
    (run / 'compiler.o').write_bytes(b'must not be uploaded')
    (candidate / 'prefix').mkdir()
    (candidate / 'prefix/objc3-sdk.json').write_text('must not be uploaded')
    (candidate / 'build-report.json').write_text(json.dumps({'status': 'FAIL', 'commands': [
        {'name': 'native-validation', 'command': ['python', '--run-id', 'current-run']}]}))
    target = tmp_path / 'evidence'
    assert preserve(repo, target, before) == ['current']
    assert (target / 'native-validation/current-run/lookup.log').read_text() == 'actual preprocessing diagnostic'
    assert not list(target.rglob('compiler.o')) and not list(target.rglob('objc3-sdk.json'))
    assert preserve(repo, tmp_path / 'unchanged', snapshot(repo)) == []


def test_sdk_diagnostics_reject_run_identity_escaping_owned_output(tmp_path):
    from scripts.objc3c_assurance.package_evidence import preserve
    candidate = tmp_path / 'out/toolchains/linux-sdk-builds/current'
    candidate.mkdir(parents=True)
    (candidate / 'build-report.json').write_text(json.dumps({'commands': [
        {'name': 'native-validation', 'command': ['python', '--run-id', '../unowned']}]}))
    with pytest.raises(ValueError, match='unowned run identity'):
        preserve(tmp_path, tmp_path / 'evidence', {})


def test_native_artifact_inventory_includes_retained_product_bytes(tmp_path):
    root = tmp_path / 'sanitizers'
    shared = root / 'campaign/product-evidence-blobs' / ('a'*64+'.zst')
    private = root / 'structured/product-evidence/blobs' / ('b'*64+'.zst')
    for blob in (shared, private):
        blob.parent.mkdir(parents=True)
        blob.write_bytes(b'actual retained native input')
    inventory = evidence.native_files(root)
    assert shared in inventory and private in inventory
    assert all(evidence.artifact(tmp_path, blob)['bytes'] == len(b'actual retained native input')
               for blob in (shared, private))


def test_sanitizer_product_gate_requires_real_deduplicated_campaign_store(tmp_path):
    campaign = tmp_path / 'sanitizer-products/campaign'
    blobs = campaign / 'product-evidence-blobs';blobs.mkdir(parents=True)
    blob = blobs / ('a'*64+'.zst');blob.write_bytes(b'x')
    expected = {'structured', 'native-async', 'coroutines', 'c-frame-bridge', 'installed-ownership',
        *(kind+'-'+sanitizer+'-'+optimization for kind in ('features', 'factories')
          for sanitizer in ('address-undefined', 'thread') for optimization in ('O0', 'O2'))}
    report = {'status': 'PASS', 'owners': [{'name': name, 'status': 'PASS'} for name in expected],
        'expected_owners': list(expected), 'runtime_profiles': [{} for _ in range(6)],
        'product_evidence_store': {
            'schema': 'objc3c.product-evidence-shared-store.v1', 'owners': 13, 'references': 1,
            'private_bytes': 2, 'stored_bytes': 1, 'deduplicated_bytes': 1, 'path': 'product-evidence-blobs'}}
    receipt = campaign / 'qualification.json';receipt.write_text(json.dumps(report))
    assert len(evidence.suite_results(tmp_path, 'sanitizer-products')) == 13
    report['product_evidence_store']['deduplicated_bytes'] = 0
    receipt.write_text(json.dumps(report))
    with pytest.raises(ValueError, match='owner/profile matrix'):
        evidence.suite_results(tmp_path, 'sanitizer-products')


def test_actual_shared_product_store_survives_upload_without_private_symlinks(tmp_path):
    from compression import zstd
    import hashlib
    from scripts.objc3c_toolchain.product_evidence import blob_bytes, consolidate_campaign_blobs
    producer = tmp_path / 'original-runner/results'
    campaign = producer / 'sanitizer-products/campaign'
    names = ['structured', 'native-async', 'coroutines', 'c-frame-bridge', 'installed-ownership',
        *(kind+'-'+sanitizer+'-'+optimization for kind in ('features', 'factories')
          for sanitizer in ('address-undefined', 'thread') for optimization in ('O0', 'O2'))]
    payload = b'exact retained replay source bytes'
    signature = hashlib.sha256(payload).hexdigest();packed = zstd.compress(payload)
    record = {'blob': 'blobs/'+signature+'.zst', 'sha256': signature, 'bytes': len(payload),
        'encoding': 'zstd', 'stored_bytes': len(packed), 'stored_sha256': hashlib.sha256(packed).hexdigest()}
    for name in names:
        root = campaign / name / 'product-evidence';(root / 'blobs').mkdir(parents=True)
        (root / record['blob']).write_bytes(packed)
        (root / 'qualification.json').write_text(json.dumps({
            'schema': 'objc3c.sanitizer-product-evidence.v1', 'status': 'PASS', 'records': [record]}))
    store = consolidate_campaign_blobs(campaign)
    assert store['path'] == 'product-evidence-blobs'
    report = {'status': 'PASS', 'owners': [{'name': name, 'status': 'PASS'} for name in names],
        'expected_owners': names, 'runtime_profiles': [{} for _ in range(6)], 'product_evidence_store': store}
    (campaign / 'qualification.json').write_text(json.dumps(report))
    assert len(evidence.suite_results(producer, 'sanitizer-products')) == 13
    archive = tmp_path / 'native-results.zip'
    with zipfile.ZipFile(archive, 'w') as output:
        for path in evidence.native_files(producer / 'sanitizer-products'):
            output.write(path, path.relative_to(producer))
    shutil.rmtree(producer.parent)
    gate = tmp_path / 'relocated-gate/evidence'
    with zipfile.ZipFile(archive) as contents: contents.extractall(gate)
    assert len(evidence.suite_results(gate, 'sanitizer-products')) == 13
    for name in names:
        root = gate / 'sanitizer-products/campaign' / name / 'product-evidence'
        assert not (root / 'blobs').exists()
        assert blob_bytes(root, record) == payload
    # An invalid namespace cannot redirect the reader even with outer hashes
    # recomputed. The only admitted store is the local campaign-owned directory.
    receipt = gate / 'sanitizer-products/campaign/qualification.json'
    for path in ('../unowned', '/original/runner/product-evidence-blobs',
                 'product-evidence-blobs/../unowned'):
        report['product_evidence_store']['path'] = path
        receipt.write_text(json.dumps(report))
        with pytest.raises(ValueError, match='owner/profile matrix'):
            evidence.suite_results(gate, 'sanitizer-products')


@pytest.mark.parametrize('paths', [[], None, ['native/objc3c/src/runtime/x.cpp'],
    ['toolchains/llvm/patches/fix.patch'], ['docs/support/evidence.md'], ['docs/contracts/language.md'],
    ['mystery.new'], ['README.md', 'tests/new.cpp'], ['native/deleted.cpp', 'docs/moved.md']])
def test_nonprose_or_unknown_changes_never_drop_native_execution(paths):
    plan = select(paths, **REV)
    assert {'foundation', 'runtime', 'frontend', 'installed'} <= {suite['id'] for suite in plan['suites']}
    validate(plan)


def test_prose_exclusion_is_explicit_and_periodic_never_uses_it():
    plan = select(['README.md', 'docs/guide.md'], **REV)
    assert plan['classification'] == 'prose-only' and plan['reasons'] and not plan['suites']
    assert len(select(['docs/guide.md'], **REV, periodic=True)['suites']) >= 6
    assert evidence.aggregate(plan, Path('/not-needed'), outcomes={'plan': 'success', 'native': 'skipped'},
                              tested=REV['tested'])['status'] == 'PASS'
    with pytest.raises(ValueError):
        evidence.aggregate(plan, Path('/not-needed'), outcomes={'plan': 'success', 'native': 'success'},
                           tested=REV['tested'])


def test_periodic_and_owner_selection_include_new_native_qualification_suites():
    periodic = select(['README.md'], **REV, periodic=True)
    expected = {'foundation', 'runtime', 'frontend', 'installed', 'sanitizers', 'upstream',
        'swift-consumer', 'semantic-workloads', 'package', 'sanitizer-products',
        'runtime-schedules', 'frontend-fuzz', 'source-coverage'}
    assert {row['id'] for row in periodic['suites']} == expected
    assert 'frontend-fuzz' in {row['id'] for row in select(['native/fuzz/FrontendTarget.cpp'], **REV)['suites']}
    assert 'runtime-schedules' in {row['id'] for row in select([
        'tests/clang_foundation/runtime_schedules/history.cpp'], **REV)['suites']}
    assert 'source-coverage' in {row['id'] for row in select([
        'scripts/objc3c_toolchain/compiler_source_coverage.py'], **REV)['suites']}


def test_native_assurance_workflow_has_exact_periodic_and_dispatch_revision_gate():
    root = Path(__file__).resolve().parents[2]
    workflow = (root / '.github/workflows/native-assurance.yml').read_text()
    assert 'schedule:' in workflow and 'workflow_dispatch:' in workflow
    assert '--periodic-sha "$GITHUB_SHA"' in workflow
    assert workflow.count('--periodic-sha "$GITHUB_SHA"') == 2
    assert 'objc3c_assurance plan' in workflow and 'objc3c_assurance verify' in workflow
    dependencies = {'gnupg', 'libxml2', 'libcurl4', 'libedit2', 'libsqlite3-0', 'libncurses6'}
    for path in ('native-assurance.yml', 'adaptive-closeout.yml'):
        text = (root / '.github/workflows' / path).read_text()
        assert all(name in text for name in dependencies)


def test_frontend_fuzz_evidence_requires_patched_frontend_feedback(tmp_path):
    root = tmp_path / 'frontend-fuzz/campaign';root.mkdir(parents=True)
    campaigns = [{'strategy': name, 'status': 'PASS', 'executions': 8} for name in
                 ('raw-dictionary', 'structured-pair', 'frontend-syntax-sema')]
    campaigns[-1]['covered_patched_frontend_functions'] = {'Parse': ['p'], 'Sema': ['s']}
    controls = [{'name': str(index), 'status': 'PASS'} for index in range(6)]
    (root / 'qualification.json').write_text(json.dumps({'status': 'PASS', 'campaigns': campaigns,
        'controls': controls, 'foundation_feedback': {'libclang-cpp.so': {}, 'libLLVM.so': {}},
        'frontend_import_fixture': {'canonical_name': 'Fuzz.Dependency'},
        'temporary_build_reclaimed': True}))
    assert len(evidence.suite_results(tmp_path, 'frontend-fuzz')) == 9
    campaigns[-1]['covered_patched_frontend_functions'].pop('Sema')
    (root / 'qualification.json').write_text(json.dumps({'status': 'PASS', 'campaigns': campaigns,
        'controls': controls, 'foundation_feedback': {'libclang-cpp.so': {}, 'libLLVM.so': {}},
        'frontend_import_fixture': {'canonical_name': 'Fuzz.Dependency'},
        'temporary_build_reclaimed': True}))
    with pytest.raises(ValueError, match='fuzz campaigns'):
        evidence.suite_results(tmp_path, 'frontend-fuzz')


@pytest.mark.parametrize('paths', [['scripts/objc3c_toolchain/build.py'],
    ['native/objc3c/src/runtime/service.cpp'], ['tests/clang_foundation/codegen.mm'], None])
def test_codegen_and_runtime_changes_select_actual_sanitizer_owner(paths):
    plan = select(paths, **REV)
    assert any(row['id'] == 'sanitizers' and 'sanitizer-detector-control' in row['execution'] for row in plan['suites'])


def test_changed_foundation_cannot_select_the_old_compiler():
    assert select(['toolchains/llvm/cmake/new.cmake'], **REV)['foundation_policy'] == 'rebuild-required'
    assert select(['native/objc3c/src/clang_foundation/x.cpp'], **REV)['foundation_policy'] == 'verify-and-incrementally-build'


@pytest.mark.parametrize('change', ['empty', 'wrong-sha', 'classification', 'digest'])
def test_modified_or_empty_plan_is_not_admitted(change):
    plan = select(['native/x.cpp'], **REV)
    if change == 'empty':
        plan['suites'] = []
    elif change == 'wrong-sha':
        plan['revision']['tested'] = 'd'*40
    elif change == 'classification':
        plan['classification'] = 'prose-only'
    else:
        plan['sha256'] = '0'*64
    with pytest.raises(ValueError):
        validate(plan)


def passing_bundle(tmp_path):
    plan = select(['tests/tooling/contract.py'], **REV)
    rows = []
    for suite in plan['suites']:
        root = tmp_path / suite['id']
        root.mkdir()
        if suite['id'] == 'foundation':
            (root / 'foundation-0.json').write_text(json.dumps({'tests': [{'name': 'parser rejection', 'code': 'PASS'}]}))
            source = {'status': 'passed', 'phases': [dict(phase=name, exit_code=0, **(
                {'test_results': '/producer/foundation-0.json'} if name == 'test' else {}))
                for name in ('configure', 'build', 'test', 'stage')]}
            (root / 'foundation.json').write_text(json.dumps(source))
        elif suite['id'] == 'runtime':
            (root / 'runtime.json').write_text('{"status":"passed"}')
            (root / 'runtime-ctest-inventory.json').write_text('["runtime"]')
            (root / 'runtime-ctest.xml').write_text('<testsuite><testcase name="runtime"/></testsuite>')
        elif suite['id'] == 'frontend':
            (root / 'frontend-tests').mkdir()
            (root / 'frontend-tests/qualification.json').write_text(json.dumps({'status': 'PASS',
                'ctest_test_names': ['objc3_x'], 'frontend_ctests': 1, 'tests': [{'name': 'objc3_x', 'sha256': '0'*64}]}))
            (root / 'frontend-objc3_x-ctest.xml').write_text('<testsuite><testcase name="objc3_x"/></testsuite>')
        else:
            (root / 'installed-tests').mkdir()
            (root / 'installed-tests/summary.json').write_text(json.dumps({'status': 'PASS',
                'commands': [{'name': 'execute', 'exit_code': 0}]}))
        rows.append({'id': suite['id'], 'status': 'PASS', 'executions': evidence.suite_results(tmp_path, suite['id']),
                     'artifacts': [evidence.artifact(tmp_path, p) for p in sorted(root.rglob('*')) if p.is_file()]})
    result = {'schema': evidence.SCHEMA, 'status': 'PASS', 'revision': plan['revision'],
              'plan_sha256': plan['sha256'], 'suites': rows, 'commands': [{'exit_code': 0}]}
    (tmp_path / 'result.json').write_text(json.dumps(result))
    return plan, result


def verify(selected_plan, tmp_path, **outcomes):
    return evidence.aggregate(selected_plan, tmp_path, outcomes={'plan': 'success', 'native': 'success', **outcomes}, tested=REV['tested'])


def test_complete_bundle_survives_producer_path_relocation(tmp_path):
    plan, _ = passing_bundle(tmp_path)
    assert verify(plan, tmp_path)['status'] == 'PASS'
    assert verify(plan, tmp_path)['suites'] == ['foundation', 'frontend', 'installed', 'runtime']


@pytest.mark.parametrize('fault', ['missing-receipt', 'wrong-sha', 'wrong-plan', 'incomplete', 'failed',
    'missing-suite', 'duplicate-suite', 'no-execution', 'no-artifact', 'missing-artifact', 'changed-artifact',
    'traversal', 'symlink', 'native-cancelled', 'native-skipped', 'native-failed', 'plan-failed',
    'failed-command', 'inner-failure', 'empty-native-results', 'unexpected-skip'])
def test_required_gate_rejects_fault_injections(tmp_path, fault):
    plan, result = passing_bundle(tmp_path)
    outcomes = {}
    if fault == 'missing-receipt':
        (tmp_path / 'result.json').unlink()
    elif fault == 'wrong-sha':
        result['revision'] = {**REV, 'tested': 'd'*40}
    elif fault == 'wrong-plan':
        result['plan_sha256'] = '0'*64
    elif fault in ('incomplete', 'failed'):
        result['status'] = 'running' if fault == 'incomplete' else 'FAIL'
    elif fault == 'missing-suite':
        result['suites'].pop()
    elif fault == 'duplicate-suite':
        result['suites'][-1] = deepcopy(result['suites'][0])
    elif fault == 'no-execution':
        result['suites'][0]['executions'] = []
    elif fault == 'no-artifact':
        result['suites'][0]['artifacts'] = []
    elif fault in ('missing-artifact', 'changed-artifact', 'symlink'):
        path = tmp_path / result['suites'][0]['artifacts'][0]['path']
        path.unlink()
        if fault == 'changed-artifact':
            path.write_text('substituted cached result')
        elif fault == 'symlink':
            path.symlink_to(tmp_path / 'result.json')
    elif fault == 'traversal':
        result['suites'][0]['artifacts'][0]['path'] = '../outside.json'
    elif fault.startswith('native-'):
        outcomes['native'] = fault.removeprefix('native-').replace('failed', 'failure')
    elif fault == 'plan-failed':
        outcomes['plan'] = 'failure'
    elif fault == 'failed-command':
        result['commands'][0]['exit_code'] = 1
    else:
        path = tmp_path / ('runtime/runtime-ctest.xml' if fault == 'unexpected-skip' else 'foundation/foundation-0.json')
        path.write_text('<testsuite><testcase name="runtime"><skipped/></testcase></testsuite>' if fault == 'unexpected-skip'
                        else json.dumps({'tests': [] if fault == 'empty-native-results' else [{'name': 'bad', 'code': 'FAIL'}]}))
        # Even an updated content hash cannot turn an inner failure into a pass.
        suite = next(row for row in result['suites'] if row['id'] == path.parent.name)
        suite['artifacts'] = [evidence.artifact(tmp_path, tmp_path / row['path']) for row in suite['artifacts']]
    if fault != 'missing-receipt':
        (tmp_path / 'result.json').write_text(json.dumps(result))
    with pytest.raises((ValueError, OSError)):
        verify(plan, tmp_path, **outcomes)


def test_actual_git_revision_and_deleted_source_drive_selection(tmp_path):
    def git(*args):
        return subprocess.check_output(['git', '-C', str(tmp_path), *args], text=True).strip()
    git('init', '-q')
    git('config', 'user.name', 'Assurance Test')
    git('config', 'user.email', 'assurance@example.invalid')
    (tmp_path / 'native.cpp').write_text('int x;')
    git('add', '.'); git('commit', '-qm', 'base')
    base = git('rev-parse', 'HEAD')
    (tmp_path / 'native.cpp').unlink()
    (tmp_path / 'README.md').write_text('moved implementation')
    git('add', '-A'); git('commit', '-qm', 'delete code')
    head = git('rev-parse', 'HEAD')
    assert from_checkout(tmp_path, head=head, base=base, tested=head)['classification'] == 'native'
    with pytest.raises(ValueError, match='tested SHA'):
        from_checkout(tmp_path, head=head, base=base, tested=base)
    (tmp_path / 'README.md').write_text('dirty')
    with pytest.raises(ValueError, match='tracked source'):
        from_checkout(tmp_path, head=head, base=base, tested=head)


@pytest.mark.parametrize('fault', ['good', 'compile', 'runtime', 'missing-binary', 'skip', 'empty', 'new-test'])
def test_real_native_commands_and_ctest_fail_closed(tmp_path, fault):
    """Compile and execute real C; no mock may satisfy this boundary control."""
    source = tmp_path / 'src'; source.mkdir()
    build = tmp_path / 'build'
    output = tmp_path / 'evidence'; output.mkdir()
    (source / 'main.c').write_text('this is invalid C' if fault == 'compile' else
                                'int main(void) { return ' + ('1' if fault == 'runtime' else '0') + '; }')
    tests = '' if fault == 'empty' else 'add_test(NAME witness COMMAND witness)\n'
    if fault == 'skip':
        tests += 'set_tests_properties(witness PROPERTIES SKIP_RETURN_CODE 0)\n'
    if fault == 'new-test':
        tests += 'add_test(NAME added_later COMMAND witness)\n'
    (source / 'CMakeLists.txt').write_text('cmake_minimum_required(VERSION 3.24)\n'
        'project(assurance_fault C)\nenable_testing()\nadd_executable(witness main.c)\n' + tests)
    records = []
    def command(label, argv):
        checked_command(label, argv, repo=tmp_path, environment=os.environ.copy(), directory=output, records=records)
    command('configure', ['cmake', '-S', source, '-B', build, '-G', 'Ninja'])
    if fault == 'compile':
        with pytest.raises(ValueError, match='native command failed'):
            command('compile', ['cmake', '--build', build])
        assert records[-1]['exit_code'] != 0 and (output / 'compile.log').is_file()
        return
    command('compile', ['cmake', '--build', build])
    if fault == 'missing-binary':
        (build / 'witness').unlink()
    if fault in ('good', 'new-test'):
        rows = execute_ctest(build, 'actual', output, command)
        assert {row['name'] for row in rows} == ({'witness', 'added_later'} if fault == 'new-test' else {'witness'})
    else:
        with pytest.raises(ValueError):
            execute_ctest(build, 'actual', output, command)
