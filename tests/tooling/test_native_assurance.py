"""Fault controls for revision selection, real native execution and terminal gates."""
from copy import deepcopy
import json
import os
from pathlib import Path
import subprocess
import shutil
import zipfile
import sys
import time
import xml.etree.ElementTree as ET

import pytest

from scripts.objc3c_assurance import evidence
from scripts.objc3c_assurance.execution import checked_command, execute_ctest
from scripts.objc3c_assurance.plan import from_checkout, select, validate
from scripts.objc3c_assurance.runner import package_execution

REV = dict(head='a'*40, base='b'*40, tested='c'*40)


@pytest.mark.parametrize('interrupted', [False, True])
def test_native_timeout_and_interruption_kill_compiler_descendant_sessions(tmp_path, monkeypatch, interrupted):
    """A compiler can launch a child in a separate process group, as Ninja does."""
    from scripts.objc3c_assurance import execution
    from scripts.objc3c_toolchain.binary_packages import digest
    marker = tmp_path / 'child.pid'
    child = ('import os,time;os.setpgrp();'
             'open('+repr(str(marker))+',"w").write(str(os.getpid()));time.sleep(60)')
    parent = ('import subprocess,sys,time;'
              'subprocess.Popen([sys.executable,"-c",'+repr(child)+']);time.sleep(60)')
    original_wait, calls = subprocess.Popen.wait, []
    if interrupted:
        def interrupt_once(process, *args, **kwargs):
            if not calls:
                calls.append(process.pid)
                deadline = time.monotonic()+5
                while not marker.exists() and time.monotonic() < deadline:
                    time.sleep(.01)
                assert marker.exists(), 'descendant failed to start before interruption'
                raise KeyboardInterrupt('controlled native interruption')
            return original_wait(process, *args, **kwargs)
        monkeypatch.setattr(subprocess.Popen, 'wait', interrupt_once)
    records = []
    with pytest.raises(KeyboardInterrupt if interrupted else ValueError):
        execution.checked_command('bounded-native', [sys.executable, '-c', parent],
            repo=tmp_path, environment=dict(os.environ), directory=tmp_path,
            records=records, timeout=5 if interrupted else .5)
    assert marker.exists()
    pid = int(marker.read_text())
    # Grandchildren may briefly remain zombies under a container's PID 1;
    # no compiler work may survive in either process group.
    stat = Path('/proc') / str(pid) / 'stat'
    assert not stat.exists() or stat.read_text().split(') ', 1)[1].startswith('Z ')
    row = records[0]
    assert row['elapsed_seconds'] > 0 and row['error']
    if not interrupted:
        assert row['exit_code'] == 124 and row['timed_out']
        assert row['log_sha256'] == digest(Path(row['log']))


@pytest.mark.parametrize('filename', ['fresh.log', 'fresh.xml'])
def test_native_commands_reject_previous_log_and_ctest_results(tmp_path, filename):
    (tmp_path / filename).write_text('previous success')
    with pytest.raises(ValueError, match='fresh log and CTest'):
        checked_command('fresh', ['ctest'], repo=tmp_path, environment=dict(os.environ),
                        directory=tmp_path, records=[])


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


def test_product_consumers_keep_the_exact_sdk_immutable_and_skip_development_scanner():
    source = (Path(__file__).resolve().parents[2] /
              'scripts/objc3c_assurance/runner.py').read_text()
    assert "verify_sdk(sdk, report['product']['seal_sha256'])" in source
    assert "'-DOBJC3C_BUILD_SCAN_DEPS=OFF'" in source
    assert "'objc3c', 'clang-scan-deps'" not in source
    assert "command('install', ['cmake', '--install', build, '--prefix', sdk])" not in source


def test_source_coverage_reader_derives_current_compiler_suite_inventory(tmp_path, monkeypatch):
    from scripts.objc3c_assurance import emitter_shards
    root = tmp_path / 'source-coverage'
    root.mkdir()
    suite = ET.Element('testsuite', tests='4', failures='0')
    for index in range(4):
        ET.SubElement(suite, 'testcase', name='merge-'+str(index))
    ET.ElementTree(suite).write(root / 'profile-merge-regressions.xml')
    (root / 'controls').mkdir();(root / 'first-party').mkdir();(root / 'compiler').mkdir()
    (root / 'controls/qualification.json').write_text(json.dumps({
        'status': 'PASS', 'controls': [{'status': 'PASS'} for _ in range(5)]}))
    native_suites = [{'name': 'native-'+str(index), 'component': 'frontend', 'status': 'PASS',
                      'source_coverage': {'source.cpp': {'state': 'executed'}}} for index in range(13)]
    (root / 'first-party/qualification.json').write_text(json.dumps({
        'status': 'PASS', 'emitter_partition': {'proof': 'exact'}, 'suites': native_suites,
        'temporary_build_reclaimed': True}))
    units = {'/build/bin/unit-'+str(index): 'a'*40 for index in range(5)}
    compiler_suites = [{'name': 'ObjC3-lit', 'status': 'PASS'}] + [
        {'name': Path(path).name, 'status': 'PASS'} for path in units]
    (root / 'compiler/qualification.json').write_text(json.dumps({
        'status': 'PASS', 'suites': compiler_suites, 'executed_unit_binary_ids': units,
        'eligible': {'clang/lib/source.cpp': 'a'*64},
        'source_coverage': {'clang/lib/source.cpp': {'state': 'executed'}},
        'binary_ids': ['b'*40]}))
    monkeypatch.setattr(emitter_shards, 'EMITTER_TESTS', ())
    monkeypatch.setattr(emitter_shards, 'emitter_partition', lambda *_: {'proof': 'exact'})
    assert len(evidence.suite_results(tmp_path, 'source-coverage')) == 2 + 13 + 6
    compiler_suites.pop()
    (root / 'compiler/qualification.json').write_text(json.dumps({
        'status': 'PASS', 'suites': compiler_suites, 'executed_unit_binary_ids': units,
        'eligible': {'clang/lib/source.cpp': 'a'*64},
        'source_coverage': {'clang/lib/source.cpp': {'state': 'executed'}},
        'binary_ids': ['b'*40]}))
    with pytest.raises(ValueError, match='source coverage is incomplete'):
        evidence.suite_results(tmp_path, 'source-coverage')


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
            source = {'status': 'passed', 'source_identity': 'fixture', 'elapsed_seconds': 4.1,
                'cost_context': {'schema': 'objc3c.foundation-cost-context.v1', 'build_config_digest': 'fixture',
                    'configuration_input_identity': 'e'*64,
                    'instrumentation': None, 'host_environment': {'compiler': 'fixture'}, 'host': {'system': 'fixture'},
                    'initial_state': 'cold-build-directory', 'initial_cache_files': {
                        'CMakeCache.txt': None, 'build.ninja': None, '.ninja_log': None},
                    'initial_entry_count': 0,
                    'source_preparation': {'performed': True, 'seconds': .1}, 'cache_scope': 'shared caches'},
                'phases': []}
            for index, name in enumerate(('configure', 'build', 'test', 'stage')):
                log = root / ('phase-'+str(index)+'.log');log.write_text('actual command fixture log')
                source['phases'].append(dict(phase=name, exit_code=0, command=[name], plan_identity=name,
                    log='/producer/'+name+'.log', log_sha256=evidence.digest(log), elapsed_seconds=1.0, **(
                    {'test_results': '/producer/foundation-0.json'} if name == 'test' else {})))
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
    from scripts.objc3c_toolchain.semantic_build_costs import snapshot
    costs = tmp_path / 'operational-costs.json'
    costs.write_text(json.dumps(snapshot(foundations=[tmp_path / 'foundation/foundation.json'])))
    result = {'operational_costs': evidence.artifact(tmp_path, costs),
              'schema': evidence.SCHEMA, 'status': 'PASS', 'revision': plan['revision'],
              'plan_sha256': plan['sha256'], 'suites': rows, 'commands': [{'exit_code': 0}]}
    (tmp_path / 'result.json').write_text(json.dumps(result))
    return plan, result


def verify(selected_plan, tmp_path, **outcomes):
    return evidence.aggregate(selected_plan, tmp_path, outcomes={'plan': 'success', 'native': 'success', **outcomes}, tested=REV['tested'])


def test_execution_shards_partition_all_selected_suites_and_profile_builds(tmp_path):
    from scripts.objc3c_assurance.plan import execution_shards, selected_suites, foundation_receipts
    plan = select(None, **REV, periodic=True)
    assert execution_shards(plan) == ['core', 'frontend-fuzz', 'source-coverage']
    groups = {name: selected_suites(plan, name) for name in execution_shards(plan)}
    names = [row['id'] for suites in groups.values() for row in suites]
    assert len(names) == len(set(names)) == len(plan['suites'])
    assert set(names) == {row['id'] for row in plan['suites']}
    core = [row['id'] for row in groups['core']]
    assert core.index('package') < core.index('sanitizer-products') < core.index('runtime-schedules')
    receipts = foundation_receipts(tmp_path, groups['core'])
    assert receipts == [tmp_path / 'foundation/foundation.json',
        tmp_path / 'sanitizer-products/foundations/compiler-asan-ubsan/foundation.json']
    assert foundation_receipts(tmp_path, groups['frontend-fuzz']) == [
        tmp_path / 'frontend-fuzz/foundations/compiler-fuzz-asan-ubsan/foundation.json']
    with pytest.raises(ValueError, match='shard'):
        selected_suites(select(['tests/tooling/contract.py'], **REV), 'frontend-fuzz')
    with pytest.raises(ValueError, match='shard'):
        selected_suites(plan, 'unknown')
    assert execution_shards(select(['docs/roadmap/prose.md'], **REV)) == []


def test_release_matrix_partitions_the_complete_core_once_after_one_product_producer():
    from scripts.objc3c_assurance.plan import selected_suites
    from scripts.objc3c_assurance.release_matrix import CONSUMER_GROUPS, GROUPS
    plan = select(None, **REV, periodic=True)
    expected = {row['id'] for row in selected_suites(plan, 'core')}
    assigned = [suite for suites in GROUPS.values() for suite in suites]
    assert len(assigned) == len(set(assigned)) and set(assigned) == expected
    assert GROUPS['product-producer'] == ('package', 'upstream')
    assert GROUPS['asan-products'] == ('sanitizer-products',)
    assert GROUPS['runtime-schedules'] == ('runtime-schedules',)
    assert set(CONSUMER_GROUPS) == set(GROUPS)-{'product-producer'}


def release_consumer_fixture(tmp_path, monkeypatch):
    from scripts.objc3c_assurance import runner
    repo = tmp_path / 'repo';repo.mkdir()
    bootstrap = tmp_path / 'bootstrap';(bootstrap / 'bin').mkdir(parents=True)
    (bootstrap / 'bin/clang').write_bytes(b'clang')
    (bootstrap / 'bin/ld.lld').write_bytes(b'linker')
    monkeypatch.setenv('OBJC3C_BOOTSTRAP_LLVM_ROOT', str(bootstrap))
    sdk = tmp_path / 'sdk';(sdk / 'lib').mkdir(parents=True)
    source, seal, archive = 's'*64, 'e'*64, 'a'*64
    (sdk / 'objc3-sdk.json').write_text(json.dumps({
        'source_build_id': source, 'seal_sha256': seal,
        'target': 'x86_64-unknown-linux-gnu'}))
    release = tmp_path / 'release.json'
    release.write_text(json.dumps({'status': 'PASS', 'seal_sha256': seal,
                                    'archive_sha256': archive}))
    plan = {'revision': dict(REV), 'periodic': True, 'sha256': 'p'*64,
            'cache_policy': 'fixture', 'suites': [{'id': 'installed'}]}
    monkeypatch.setattr(runner, 'validate', lambda _: None)
    monkeypatch.setattr(runner, 'from_checkout', lambda *args, **kwargs: plan)
    monkeypatch.setattr(runner, 'execute_suites', lambda *args, **kwargs: None)
    return runner, repo, sdk, release, plan, {'source_build_id': source,
                                             'seal_sha256': seal}


def test_release_consumer_admits_and_rechecks_the_exact_installed_sdk(tmp_path, monkeypatch):
    runner, repo, sdk, release, plan, installed = release_consumer_fixture(tmp_path, monkeypatch)
    calls = []
    monkeypatch.setattr(runner, 'verify_sdk',
        lambda prefix, seal: calls.append((prefix, seal)) or installed)
    output = tmp_path / 'result'

    result = runner.run(repo, plan, output, suite_ids=('installed',),
                        product_sdk=sdk, package_release=release)

    assert result['status'] == 'PASS'
    assert calls == [(sdk, installed['seal_sha256'])] * 2
    assert json.loads((output / 'result.json').read_text())['status'] == 'PASS'


def test_release_consumer_preflight_failure_retains_uploadable_receipt(tmp_path, monkeypatch):
    runner, repo, sdk, release, plan, _ = release_consumer_fixture(tmp_path, monkeypatch)
    monkeypatch.setattr(runner, 'verify_sdk',
                        lambda *args: (_ for _ in ()).throw(ValueError('seal mismatch')))
    output = tmp_path / 'result'

    with pytest.raises(ValueError, match='seal mismatch'):
        runner.run(repo, plan, output, suite_ids=('installed',),
                   product_sdk=sdk, package_release=release)

    receipt = json.loads((output / 'result.json').read_text())
    assert receipt['status'] == 'FAIL'
    assert receipt['error'] == 'ValueError: seal mismatch'


def test_release_matrix_preflight_failure_retains_uploadable_receipt(tmp_path, monkeypatch):
    from scripts.objc3c_assurance import release_matrix
    repo = tmp_path / 'repo';(repo / 'out').mkdir(parents=True)
    plan = {'revision': dict(REV), 'sha256': 'p' * 64}
    monkeypatch.setattr(release_matrix, 'validate', lambda _: None)
    monkeypatch.setattr(release_matrix, '_require_checkout',
                        lambda *_: (_ for _ in ()).throw(ValueError('revision mismatch')))
    output = repo / 'out/release-matrix/results/frontend'

    with pytest.raises(ValueError, match='revision mismatch'):
        release_matrix.run_group(repo, plan, output, 'frontend',
                                 product_sdk=tmp_path / 'sdk',
                                 package_release=tmp_path / 'release.json')

    receipt = json.loads((output / 'result.json').read_text())
    assert receipt['status'] == 'FAIL'
    assert receipt['release_matrix_group'] == 'frontend'
    assert receipt['error'] == 'ValueError: revision mismatch'


def test_release_consumer_parallelism_does_not_require_producer_source_tree(tmp_path):
    from scripts.objc3c_assurance.runner import _consumer_compile_jobs
    repo = tmp_path / 'repo';(repo / 'toolchains/llvm').mkdir(parents=True)
    (repo / 'toolchains/llvm/manifest.json').write_text(json.dumps({
        'build': {'compile_jobs': 8, 'link_jobs': 4}}))

    jobs = _consumer_compile_jobs(repo)

    assert 1 <= jobs <= 8
    assert not (repo / 'out/toolchains/objc3-development/src').exists()


def test_coverage_producer_failure_retains_typed_receipt(tmp_path, monkeypatch):
    from scripts.objc3c_assurance import coverage_matrix
    repo = tmp_path / 'repo';(repo / 'out').mkdir(parents=True)
    plan = {'revision': dict(REV), 'sha256': 'p' * 64}
    monkeypatch.setattr(coverage_matrix, 'validate', lambda _: None)
    monkeypatch.setattr(coverage_matrix, '_require_checkout', lambda *_: None)
    monkeypatch.setattr(coverage_matrix.subprocess, 'run',
                        lambda *args, **kwargs: (_ for _ in ()).throw(RuntimeError('pytest failed')))
    output = repo / 'out/coverage-producer'

    with pytest.raises(RuntimeError, match='pytest failed'):
        coverage_matrix.produce(repo, plan, output, repo / 'out/profile.tar.zst')

    receipt = json.loads((output / 'producer.json').read_text())
    assert receipt['schema'] == 'objc3c.coverage-profile-producer.v1'
    assert receipt['status'] == 'FAIL'
    assert receipt['error'] == 'RuntimeError: pytest failed'


def test_coverage_partition_restore_failure_retains_typed_receipt(tmp_path, monkeypatch):
    from scripts.objc3c_assurance import coverage_matrix
    repo = tmp_path / 'repo';(repo / 'out').mkdir(parents=True)
    plan = {'revision': dict(REV), 'sha256': 'p' * 64}
    monkeypatch.setattr(coverage_matrix, 'restore',
                        lambda *args: (_ for _ in ()).throw(FileNotFoundError('profile missing')))
    output = repo / 'out/partitions/3'

    with pytest.raises(FileNotFoundError, match='profile missing'):
        coverage_matrix.run_partition(repo, plan, tmp_path / 'producer.json',
                                      tmp_path / 'profile.tar.zst', output, 3)

    receipt = json.loads((output / 'qualification.json').read_text())
    assert receipt['schema'] == 'objc3c.coverage-partition-admission.v1'
    assert receipt['status'] == 'FAIL'
    assert receipt['partition'] == {'index': 3, 'count': 8}
    assert receipt['error'] == 'FileNotFoundError: profile missing'


def test_release_workflow_fans_out_exact_product_and_coverage_consumers():
    root = Path(__file__).resolve().parents[2]
    workflow = (root / '.github/workflows/native-assurance.yml').read_text()
    assert 'partition: [0, 1, 2, 3, 4, 5, 6, 7]' in workflow
    assert 'needs: [plan, product-producer]' in workflow
    assert 'needs: [plan, product-producer, asan-foundation-producer]' in workflow
    assert 'needs: [plan, coverage-producer]' in workflow
    assert 'Prove complete disjoint core union' in workflow
    assert 'Prove complete disjoint coverage union' in workflow
    assert 'coverage-profile-${{ github.sha }}-${{ github.run_id }}-${{ github.run_attempt }}' in workflow
    assert '--package-release /producer/product-producer/package/release.json' in workflow
    assert 'OBJC3C_ASAN_FOUNDATION_IDENTITY' in workflow
    assert workflow.count("mapfile -d '' archives") == 2


def test_quick_tooling_rejects_python_syntax_and_undefined_names():
    root = Path(__file__).resolve().parents[2]
    workflow = (root / '.github/workflows/task-hygiene.yml').read_text()
    assert 'uv run ruff check --select F821 scripts tests/tooling' in workflow


def test_ci_uses_activated_manifest_owned_tool_roots():
    root = Path(__file__).resolve().parents[2]
    workflows = '\n'.join(path.read_text() for path in
                          sorted((root / '.github/workflows').glob('*.yml')))
    runner = (root / 'scripts/objc3c_assurance/runner.py').read_text()
    assert 'export OBJC3C_BOOTSTRAP_LLVM_ROOT=/opt/' not in workflows
    assert "('swift-' + swift['version'])" in runner
    assert "swift-6.4.2" not in runner


def test_instrumented_foundation_identity_is_derived_without_a_source_checkout():
    from scripts.objc3c_assurance.instrumented_foundation import PROFILE, _portable_identity
    from scripts.objc3c_toolchain.core import planned_development_identity
    root = Path(__file__).resolve().parents[2]
    source = planned_development_identity(root)
    identity = _portable_identity(root)
    assert identity['schema'] == 'objc3c.portable-foundation-identity.v1'
    assert identity['profile'] == PROFILE == 'compiler-asan-ubsan'
    assert identity['upstream_commit'] == source['upstream_commit']
    assert identity['patch_digest'] == source['patch_digest']
    assert identity['instrumentation']['kind'] == 'sanitizer'
    assert identity['paths']['stage'].endswith(
        '/out/toolchains/objc3-development/stage-linux-x86_64-compiler-asan-ubsan')


def test_instrumented_foundation_stage_replaces_ambient_loader_selection(tmp_path, monkeypatch):
    from scripts.objc3c_assurance.instrumented_foundation import _stage_environment
    stage = tmp_path / 'stage';(stage / 'bin').mkdir(parents=True);(stage / 'lib').mkdir()
    bootstrap = tmp_path / 'bootstrap';(bootstrap / 'bin').mkdir(parents=True)
    (bootstrap / 'bin/llvm-symbolizer').write_bytes(b'symbolizer')
    monkeypatch.setenv('OBJC3C_BOOTSTRAP_LLVM_ROOT', str(bootstrap))
    monkeypatch.setenv('LD_LIBRARY_PATH', '/foreign/llvm/lib')
    monkeypatch.setenv('LD_PRELOAD', '/foreign/inject.so')
    environment = _stage_environment(stage)
    assert environment['LD_LIBRARY_PATH'] == str(stage / 'lib')
    assert environment['LLVM_SYMBOLIZER_PATH'] == str(bootstrap / 'bin/llvm-symbolizer')
    assert not {'LD_PRELOAD', 'LD_AUDIT'} & set(environment)


def test_instrumented_foundation_rejects_foreign_loaded_llvm(tmp_path, monkeypatch):
    from types import SimpleNamespace
    from scripts.objc3c_assurance.instrumented_foundation import _validate_stage_execution
    stage = tmp_path / 'stage';(stage / 'bin').mkdir(parents=True);(stage / 'lib').mkdir()
    llvm = stage / 'lib/libLLVM.so.23';clang = stage / 'lib/libclang-cpp.so.23'
    llvm.write_bytes(b'llvm');clang.write_bytes(b'clang')
    foreign = tmp_path / 'foreign/libLLVM.so.23';foreign.parent.mkdir();foreign.write_bytes(b'foreign')

    def loader(argv, **_):
        name = Path(argv[-1]).name
        selected = [llvm, clang] if name.startswith('clang') else [llvm]
        return ''.join(f'{path.name} => {path} (0x1)\n' for path in selected)

    monkeypatch.setattr('scripts.objc3c_assurance.instrumented_foundation.subprocess.check_output', loader)
    monkeypatch.setattr('scripts.objc3c_assurance.instrumented_foundation.subprocess.run',
                        lambda *args, **kwargs: SimpleNamespace(returncode=0, stdout='version\n'))
    versions = _validate_stage_execution(stage, [llvm, clang], {'LD_LIBRARY_PATH': str(stage / 'lib')})
    assert set(versions) == {'clang', 'clang++', 'ld.lld', 'llvm-nm', 'llvm-ar'}

    def substituted(argv, **kwargs):
        output = loader(argv, **kwargs)
        return output.replace(str(llvm), str(foreign), 1) if Path(argv[-1]).name == 'clang' else output

    monkeypatch.setattr('scripts.objc3c_assurance.instrumented_foundation.subprocess.check_output', substituted)
    with pytest.raises(ValueError, match='foreign LLVM libraries: clang'):
        _validate_stage_execution(stage, [llvm, clang], {'LD_LIBRARY_PATH': str(stage / 'lib')})


def test_external_exact_package_receipt_unblocks_only_the_product_consumer(tmp_path):
    from scripts.objc3c_assurance.execution import execute_suites
    suites = [{'id': 'sanitizer-products'}]
    with pytest.raises(ValueError, match='earlier package'):
        execute_suites(suites, output=tmp_path / 'rejected', report={'suites': []},
                       execute=lambda *_: [], save=lambda: None)
    output = tmp_path / 'accepted';output.mkdir()
    report = {'suites': []}
    def execute(_, directory):
        (directory / 'qualification.log').write_text('exact package consumer passed')
        return [{'name': 'consumer', 'status': 'PASS'}]
    execute_suites(suites, output=output, report=report, execute=execute,
                   save=lambda: None, prerequisites={'package'})
    assert report['suites'][0]['status'] == 'PASS'


@pytest.mark.parametrize('path', ['scripts/objc3c_assurance/runner.py',
    '.github/workflows/native-assurance.yml', '.github/workflows/adaptive-closeout.yml'])
def test_orchestration_edits_select_every_affected_native_owner(path):
    full = {row['id'] for row in select(None, **REV, periodic=True)['suites']} - {'upstream'}
    assert {row['id'] for row in select([path], **REV)['suites']} == full


def test_sharded_published_bundle_retains_exact_native_evidence(tmp_path):
    from scripts.objc3c_assurance.plan import foundation_receipts, selected_suites
    producer = tmp_path / 'producer';producer.mkdir()
    plan, receipt = passing_bundle(producer)
    receipt['shard'] = 'core'
    (producer / 'result.json').write_text(json.dumps(receipt))
    # Build outputs and unrelated files never enter the upload, even with a
    # durable-looking extension. The actual evidence remains byte-for-byte bound.
    ignored = producer / 'frontend/build/compile_commands.json'
    ignored.parent.mkdir();ignored.write_text('large transient graph')
    (producer / 'unselected').mkdir()
    (producer / 'unselected/transient.log').write_text('not selected')
    destination = tmp_path / 'published/core'
    published = evidence.publish(plan, producer, destination, shard='core')
    assert published['files'] > len(plan['suites'])
    assert not (destination / 'frontend/build').exists()
    assert not (destination / 'unselected').exists()
    assert (destination / 'result.json').read_bytes() == (producer / 'result.json').read_bytes()
    result = evidence.aggregate_shards(plan, destination.parent,
        outcomes={'plan': 'success', 'native': 'success'}, tested=REV['tested'])
    assert result['status'] == 'PASS' and result['shards'] == ['core']
    assert set(result['suites']) == {row['id'] for row in plan['suites']}
    assert foundation_receipts(destination, selected_suites(plan, 'core')) == [
        destination / 'foundation/foundation.json']
    with pytest.raises(ValueError, match='fresh'):
        evidence.publish(plan, producer, destination, shard='core')


@pytest.mark.parametrize('fault', ['missing', 'extra', 'linked', 'wrong-shard', 'wrong-revision',
    'cancelled', 'missing-suite', 'changed-log', 'changed-cost'])
def test_sharded_terminal_gate_rejects_incomplete_or_substituted_evidence(tmp_path, fault):
    producer = tmp_path / 'producer';producer.mkdir()
    plan, receipt = passing_bundle(producer)
    receipt['shard'] = 'core'
    (producer / 'result.json').write_text(json.dumps(receipt))
    root = tmp_path / 'published'
    evidence.publish(plan, producer, root / 'core', shard='core')
    outcomes = {'plan': 'success', 'native': 'success'}
    if fault == 'missing':
        (root / 'core').rename(root / 'omitted')
    elif fault == 'extra':
        (root / 'foreign').mkdir()
    elif fault == 'linked':
        (root / 'core').rename(root / 'saved')
        (root / 'core').symlink_to(root / 'saved', target_is_directory=True)
    elif fault == 'wrong-shard':
        receipt['shard'] = 'frontend-fuzz'
    elif fault == 'wrong-revision':
        receipt['revision'] = {**receipt['revision'], 'tested': 'd'*40}
    elif fault == 'cancelled':
        outcomes['native'] = 'cancelled'
    elif fault == 'missing-suite':
        receipt['suites'].pop()
    elif fault == 'changed-log':
        (root / 'core/foundation/phase-0.log').write_text('substituted')
    elif fault == 'changed-cost':
        (root / 'core/operational-costs.json').write_text('{}')
    if (root / 'core').is_dir():
        (root / 'core/result.json').write_text(json.dumps(receipt))
    with pytest.raises(ValueError):
        evidence.aggregate_shards(plan, root, outcomes=outcomes, tested=REV['tested'])


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


@pytest.mark.parametrize('fault', ['missing-cost', 'changed-cost', 'false-decision', 'false-timing'])
def test_native_aggregate_recomputes_operational_costs_from_qualified_owner_receipts(tmp_path, fault):
    plan, result = passing_bundle(tmp_path)
    path = tmp_path / 'operational-costs.json'
    if fault == 'missing-cost': path.unlink()
    else:
        cost = json.loads(path.read_text())
        if fault == 'false-decision': cost['decision'] = 'performance-PASS'
        elif fault == 'false-timing': cost['records'][0]['phases_seconds']['build'] = 0
        else: cost['groups'] = []
        path.write_text(json.dumps(cost))
        if fault != 'changed-cost':
            result['operational_costs'] = evidence.artifact(tmp_path, path)
            (tmp_path / 'result.json').write_text(json.dumps(result))
    with pytest.raises(ValueError): verify(plan, tmp_path)
