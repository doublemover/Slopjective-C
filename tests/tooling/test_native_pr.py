from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path
import subprocess
from threading import Barrier

import pytest

from scripts.objc3c_assurance import pr_native, preview_download
from scripts.objc3c_assurance.evidence import SCHEMA as NATIVE_SCHEMA, artifact, native_files, suite_results
from scripts.objc3c_assurance.plan import from_checkout

RELEASE = {'seal_sha256': 'a' * 64, 'sha256': 'b' * 64, 'bytes': 123,
           'source_build_id': 'published-foundation'}


@pytest.fixture
def checkout(tmp_path):
    def git(*arguments):
        return subprocess.check_output(['git', *arguments], cwd=tmp_path, text=True).strip()

    git('init', '-q')
    git('config', 'user.name', 'Native PR Test')
    git('config', 'user.email', 'native-pr@example.org')
    manifest = tmp_path / 'docs/releases/preview-manifest.json'
    manifest.parent.mkdir(parents=True)
    manifest.write_text(json.dumps({'archives': {pr_native.native_linux_host(): RELEASE}}))
    git('add', str(manifest))
    git('commit', '-qm', 'base')
    base = git('rev-parse', 'HEAD')

    def commit(path):
        changed = tmp_path / path
        changed.parent.mkdir(parents=True, exist_ok=True)
        changed.write_text('changed\n')
        git('add', path)
        git('commit', '-qm', path)
        sha = git('rev-parse', 'HEAD')
        return {'head': sha, 'base': base, 'tested': sha}

    return tmp_path, commit


@pytest.mark.parametrize('changed,available,expected', [
    ('docs/roadmap/prose.md', True, ()),
    ('docs/contracts/consumed.md', True, pr_native.SOURCE_SUITES),
    ('native/objc3c/tests/keypath_service_test.cpp', True, pr_native.SOURCE_SUITES),
    ('.github/workflows/native-pr.yml', True, pr_native.SOURCE_SUITES),
    ('.github/workflows/native-pr.yml', False, ('foundation', *pr_native.SOURCE_SUITES)),
    ('toolchains/llvm/cmake/ObjC3LLVMCache.cmake', True, ('foundation', *pr_native.SOURCE_SUITES)),
    ('tests/clang_foundation/runtime.m', False, ('foundation', *pr_native.SOURCE_SUITES)),
])
def test_pr_gate_selects_exact_revision_and_only_admitted_foundation(
        checkout, monkeypatch, changed, available, expected):
    repo, commit = checkout
    revision = commit(changed)
    sdk = repo / 'sdk' if available else None
    identity = {field: 'digest-' + field for field in pr_native.FOUNDATION_FIELDS}
    identity.update(target=pr_native.native_target(), seal_sha256='a'*64)
    monkeypatch.setattr(pr_native, 'verify_sdk', lambda path: identity)
    monkeypatch.setattr(pr_native, 'planned_development_identity', lambda path: {
        id_key: identity[field] for field, id_key in pr_native.FOUNDATION_FIELDS.items()})
    executed = []

    def execute(repo, plan, output, *, suite_ids, product_sdk, frontend_names):
        assert plan['revision'] == revision
        assert plan['changed_paths'] == [changed]
        assert not output.exists()
        assert product_sdk == (sdk if expected == pr_native.SOURCE_SUITES else None)
        assert frontend_names == (pr_native.FRONTEND_WITNESSES if product_sdk and changed.startswith('.github/') else None)
        executed.extend(suite_ids)
        return {'status': 'PASS', 'revision': plan['revision'], 'plan_sha256': plan['sha256'],
                'suites': [{'id': name, 'status': 'PASS', 'executions': ['witness']}
                           for name in suite_ids]}

    monkeypatch.setattr(pr_native, 'run', execute)
    pilot_calls = []
    original_pilot = pr_native.pr_pilot.execute

    def run_pilot(repo, plan, output, **kwargs):
        if not pr_native.pr_pilot.selected(plan):
            return original_pilot(repo, plan, output, **kwargs)
        assert executed == list(expected)
        assert output.name == 'implementation-pilot'
        assert kwargs['sdk'] == (sdk if expected == pr_native.SOURCE_SUITES else None)
        pilot_calls.append(plan['revision'])
        return {'status': 'PASS', 'dependent': True}

    monkeypatch.setattr(pr_native.pr_pilot, 'execute', run_pilot)
    output = repo / 'out' / 'native-pr'
    report = pr_native.execute(repo, output, **revision, sdk=sdk)
    assert report['suites'] == executed == list(expected)
    assert report['status'] == 'PASS'
    assert report['revision'] == revision
    assert pilot_calls == ([revision] if changed.startswith(('native/', 'toolchains/', '.github/')) else [])
    assert json.loads((output / 'result.json').read_text()) == report
    with pytest.raises(ValueError, match='fresh owned output'):
        pr_native.execute(repo, output, **revision, sdk=sdk)


@pytest.mark.parametrize('defect', ['revision', 'digest', 'missing', 'empty', 'failure', 'seal'])
def test_pr_gate_records_failure_for_incomplete_or_drifting_evidence(checkout, monkeypatch, defect):
    repo, commit = checkout
    revision = commit('tests/clang_foundation/runtime.m')
    foundations = iter([{'seal_sha256': 'a'*64}, {'seal_sha256': 'b'*64}])
    monkeypatch.setattr(pr_native, 'matching_foundation', lambda *arguments: next(foundations))

    def execute(repo, plan, output, *, suite_ids, product_sdk, frontend_names):
        assert frontend_names is None
        rows = [{'id': name, 'status': 'PASS', 'executions': ['witness']} for name in suite_ids]
        if defect == 'missing':
            rows.pop()
        if defect == 'empty':
            rows[0]['executions'].clear()
        if defect == 'failure':
            rows[-1]['status'] = 'FAIL'
        return {'status': 'PASS', 'revision': {} if defect == 'revision' else plan['revision'],
                'plan_sha256': '' if defect == 'digest' else plan['sha256'], 'suites': rows}

    monkeypatch.setattr(pr_native, 'run', execute)
    output = repo / 'out' / 'native-pr'
    with pytest.raises(ValueError, match='different tested plan|passing selected suite|SDK changed'):
        pr_native.execute(repo, output, **revision)
    assert json.loads((output / 'result.json').read_text())['status'] == 'FAIL'


def test_executor_analysis_finding_blocks_an_otherwise_passing_native_pr(checkout, monkeypatch):
    repo, commit = checkout
    revision = commit(str(pr_native.pr_pilot.SOURCE))
    executed = []
    monkeypatch.setattr(pr_native, 'matching_foundation', lambda *args: {'seal_sha256': 'a' * 64})

    def run(repo, plan, output, *, suite_ids, product_sdk, frontend_names):
        executed.extend(suite_ids)
        return {'status': 'PASS', 'revision': plan['revision'], 'plan_sha256': plan['sha256'],
                'suites': [{'id': name, 'status': 'PASS', 'executions': ['witness']} for name in suite_ids]}

    def analyze(repo, plan, output, **kwargs):
        assert executed == list(pr_native.SOURCE_SUITES)
        assert pr_native.pr_pilot.selected(plan)
        raise ValueError('implementation pilot found core.NullDereference')

    monkeypatch.setattr(pr_native, 'run', run)
    monkeypatch.setattr(pr_native.pr_pilot, 'execute', analyze)
    output = repo / 'out/native-pr'
    with pytest.raises(ValueError, match='pilot found core.NullDereference'):
        pr_native.execute(repo, output, **revision)
    assert json.loads((output / 'result.json').read_text())['status'] == 'FAIL'


def test_pr_foundation_requires_exact_sealed_arch_and_configuration(tmp_path, monkeypatch):
    sdk = tmp_path / 'sdk'
    record = {field: 'same-' + field for field in pr_native.FOUNDATION_FIELDS}
    record.update(target='wrong-target', seal_sha256='a'*64)
    expected = {key: record[field] for field, key in pr_native.FOUNDATION_FIELDS.items()}
    monkeypatch.setattr(pr_native, 'verify_sdk', lambda path: record)
    monkeypatch.setattr(pr_native, 'planned_development_identity', lambda path: expected)
    plan = {'foundation_policy': 'verify-and-incrementally-build'}
    with pytest.raises(ValueError, match='different machine'):
        pr_native.matching_foundation(tmp_path, plan, sdk)
    record['target'] = pr_native.native_target()
    assert pr_native.matching_foundation(tmp_path, plan, sdk) == record
    expected['build_config_digest'] = 'changed'
    assert pr_native.matching_foundation(tmp_path, plan, sdk) is None


@pytest.mark.parametrize('paths,expected', [
    (None, True),
    (['native/objc3c/tests/keypath_service_test.cpp'], False),
    (['toolchains/llvm/cmake/ObjC3LLVMCache.cmake'], False),
    (['toolchains/llvm/cmake/ObjC3LLVMCache.cmake', 'scripts/objc3c_toolchain/install_sdk.py'], True),
    (['tests/clang_foundation/installed_sdk/carrier-record-main.m'], True),
    (['scripts/objc3c_assurance/preview_download.py'], True),
    (['scripts/objc3c_toolchain/release_install.py'], True),
    (['.github/actions/linux-preview-archive-cache/action.yml'], True),
])
def test_install_witness_remains_selected_when_llvm_rebuilds(paths, expected):
    assert pr_native.needs_install_witness({'changed_paths': paths}) is expected


@pytest.mark.parametrize('paths,focused', [
    (None, False),
    (['.github/workflows/native-pr.yml', 'docs/roadmap/assurance.md'], True),
    (['scripts/objc3c_assurance/pr_native.py', 'tests/tooling/test_native_pr.py'], True),
    (['.github/workflows/native-pr.yml', 'native/objc3c/tests/runtime_lifetime_test.cpp'], False),
    (['tests/tooling/fixtures/clang_foundation/fixture.m'], False),
    (['.github/workflows/native-pr.yml', 'docs/contracts/rules.md'], False),
])
def test_frontend_probes_require_a_known_tooling_only_delta(paths, focused):
    assert pr_native.frontend_selection({'changed_paths': paths}) == (
        pr_native.FRONTEND_WITNESSES if focused else None)


@pytest.mark.parametrize('defect', [None, 'skipped', 'cancelled', 'lost-artifact', 'wrong-sha',
                                   'wrong-partition', 'unexpected-skip', 'changed-install-log',
                                   'wrong-install-output', 'missing-acquisition', 'wrong-cache-digest',
                                   'wrong-initial-identity', 'reused-initial-install', 'no-relocated-reuse'])
def test_terminal_gate_reconciles_independent_execution_evidence(checkout, monkeypatch, defect):
    repo, commit = checkout
    revision = commit('scripts/objc3c_toolchain/install_sdk.py')
    plan = from_checkout(repo, **revision)
    evidence = repo / 'out/native-pr'
    native = evidence / 'checked/execution'

    def write(path, value):
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(json.dumps(value))

    write(evidence / 'selection.json', plan)
    write(evidence / 'checked/plan.json', plan)
    foundation = {name: 'pinned-' + name for name in pr_native.FOUNDATION_FIELDS}
    foundation.update(source_build_id=RELEASE['source_build_id'], seal_sha256=RELEASE['seal_sha256'],
                      target=pr_native.native_target())
    monkeypatch.setattr(pr_native, 'planned_development_identity', lambda repo: {
        key: foundation[name] for name, key in pr_native.FOUNDATION_FIELDS.items()})
    log = evidence / 'install-witness.log'
    log.write_bytes(b'unrelated output\n' if defect == 'wrong-install-output' else preview_download.WITNESS_OUTPUT)
    cost = {'schema': 'objc3c.sdk-install-cost.v1', 'seal_sha256': RELEASE['seal_sha256'],
            'source_build_id': RELEASE['source_build_id'], 'archive_bytes': RELEASE['bytes'],
            'archive_sha256': RELEASE['sha256']}
    initial = {**cost, 'prefix_reused': defect == 'reused-initial-install'}
    if defect == 'wrong-initial-identity':
        initial['source_build_id'] = 'different-build'
    acquisition = {'source': 'cache', 'bytes': RELEASE['bytes'], 'elapsed_seconds': 0.05,
                   'sha256': '9'*64 if defect == 'wrong-cache-digest' else RELEASE['sha256']}
    write(evidence / 'preview-install.json', {'status': 'PASS', 'activated': True,
        'seal_sha256': RELEASE['seal_sha256'], 'cost': {**cost, 'prefix_reused': defect != 'no-relocated-reuse'},
        'archive_acquisition': {} if defect == 'missing-acquisition' else acquisition,
        'witness': {'source': 'native/main.objc3', 'status': 'PASS',
                    'log_sha256': artifact(evidence, log)['sha256'], 'initial_install_cost': initial}})
    write(evidence / 'checked/result.json', {'schema': pr_native.SCHEMA, 'status': 'PASS',
        'classification': 'native', 'revision': revision, 'plan_sha256': plan['sha256'],
        'foundation': 'verified-release', 'foundation_sdk': foundation, 'suites': ['runtime', 'frontend'],
        'implementation_pilot': {'status': 'NOT-SELECTED', 'reason': 'no inputs in the executor analysis pilot'}})

    def junit(path, name, *, skip=False):
        path.write_text(f'<testsuite tests="1"><testcase name="{name}">'
                        + ('<skipped/>' if skip else '') + '</testcase></testsuite>')

    runtime = native / 'runtime'
    write(runtime / 'runtime.json', {'status': 'passed'})
    write(runtime / 'runtime-ctest-inventory.json', ['runtime_smoke'])
    runtime_xml = runtime / 'runtime-ctest.xml'
    junit(runtime_xml, 'runtime_smoke')
    frontend = native / 'frontend'
    write(frontend / 'frontend-tests/qualification.json', {'status': 'PASS', 'frontend_ctests': 1,
        'ctest_test_names': ['objc3_frontend'], 'tests': [{'name': 'objc3_frontend'}]})
    junit(frontend / 'frontend-objc3_frontend-ctest.xml', 'objc3_frontend')
    executions = {name: suite_results(native, name) for name in pr_native.SOURCE_SUITES}
    if defect == 'unexpected-skip':
        junit(runtime_xml, 'runtime_smoke', skip=True)
    rows = [{'id': name, 'status': 'PASS', 'executions': executions[name],
             'artifacts': [artifact(native, path) for path in native_files(native / name)]}
            for name in pr_native.SOURCE_SUITES]
    write(native / 'result.json', {'schema': NATIVE_SCHEMA, 'status': 'PASS',
        'revision': {**revision, 'tested': 'a' * 40} if defect == 'wrong-sha' else revision,
        'plan_sha256': plan['sha256'], 'suites': rows,
        'suite_partition': ['runtime'] if defect == 'wrong-partition' else list(pr_native.SOURCE_SUITES)})
    if defect == 'lost-artifact':
        runtime_xml.unlink()
    if defect == 'changed-install-log':
        log.write_text('different install evidence\n')
    outcome = defect if defect in ('skipped', 'cancelled') else 'success'
    if defect:
        with pytest.raises(ValueError, match='not finish|stale|partition|artifacts|unexpected skip|install'):
            pr_native.verify(repo, evidence, execution=outcome, **revision)
    else:
        verified = pr_native.verify(repo, evidence, execution=outcome, **revision)
        assert verified['status'] == 'PASS' and verified['revision'] == revision
        assert set(verified['suites']) == set(pr_native.SOURCE_SUITES)


def test_terminal_gate_accepts_explicit_prose_plan_without_native_output(checkout):
    repo, commit = checkout
    revision = commit('docs/roadmap/example.md')
    root = repo / 'out/native-pr'
    pr_native.execute(repo, root / 'checked', **revision)
    (root / 'selection.json').write_bytes((root / 'checked/plan.json').read_bytes())
    assert pr_native.verify(repo, root, execution='success', **revision)['classification'] == 'prose-only'


@pytest.mark.parametrize('defect', [None, 'patch_digest', 'upstream_commit', 'source_tree', 'effective_patch'])
def test_terminal_gate_checks_full_cold_foundation_identity(checkout, monkeypatch, defect):
    repo, commit = checkout
    revision = commit('toolchains/llvm/cmake/ObjC3LLVMCache.cmake')
    plan = from_checkout(repo, **revision)
    evidence = repo / 'out/native-pr'
    foundation = {name: name + '-full' for name in pr_native.FOUNDATION_FIELDS}
    monkeypatch.setattr(pr_native, 'planned_development_identity', lambda repo: {
        key: foundation[name] for name, key in pr_native.FOUNDATION_FIELDS.items()})
    suites = ('foundation', *pr_native.SOURCE_SUITES)
    reached = []
    pilot_reached = []

    def verify_pilot(repo, plan, output, summary, *, admitted, foundation_report):
        assert plan['revision'] == revision and not admitted
        assert summary == {'status': 'PASS', 'dependent': True}
        assert foundation_report == output / 'execution/foundation/foundation.json'
        pilot_reached.append(output)

    def aggregate(plan, root, *, outcomes, tested, suite_ids, frontend_names):
        assert plan['revision'] == revision
        assert outcomes == {'plan': 'success', 'native': 'success'}
        assert tested == revision['tested']
        assert suite_ids == suites and frontend_names is None
        reached.append(root)
        return {'status': 'PASS'}

    monkeypatch.setattr(pr_native, 'aggregate', aggregate)
    monkeypatch.setattr(pr_native.pr_pilot, 'verify', verify_pilot)
    directory = evidence / 'checked/execution/foundation'
    directory.mkdir(parents=True)
    for path in (evidence / 'selection.json', evidence / 'checked/plan.json'):
        path.write_text(json.dumps(plan))
    (evidence / 'checked/result.json').write_text(json.dumps({
        'schema': pr_native.SCHEMA, 'status': 'PASS', 'revision': revision,
        'plan_sha256': plan['sha256'], 'classification': 'native',
        'foundation': 'current-source-build', 'foundation_sdk': None, 'suites': suites,
        'implementation_pilot': {'status': 'PASS', 'dependent': True}}))
    source = {**foundation, 'source_tree': 'a' * 40}
    effective = {'CLANG_OBJC3_PATCH_DIGEST': foundation['patch_digest'],
                 'CLANG_OBJC3_CONFIG_DIGEST': foundation['build_config_digest']}
    if defect == 'effective_patch':
        effective['CLANG_OBJC3_PATCH_DIGEST'] = 'other-patch'
    elif defect is not None:
        source[defect] = 'wrong'
    (directory / 'foundation.json').write_text(json.dumps({
        'source_identity': foundation['source_build_id'], 'foundation_source': source,
        'effective_configuration': effective, 'cost_context': {
            'build_config_digest': foundation['build_config_digest']}}))

    if defect is None:
        assert pr_native.verify(repo, evidence, execution='success', **revision)['status'] == 'PASS'
        assert reached == [directory.parent]
        assert pilot_reached == [directory.parent.parent]
    else:
        with pytest.raises(ValueError, match='built a different foundation'):
            pr_native.verify(repo, evidence, execution='success', **revision)
        assert reached == []
        assert pilot_reached == []


def test_focused_frontend_receipt_cannot_satisfy_full_suite_or_different_selection(tmp_path):
    frontend = tmp_path / 'frontend'
    proof = frontend / 'frontend-tests'
    proof.mkdir(parents=True)
    (proof / 'qualification.json').write_text(json.dumps({'status': 'PASS', 'frontend_ctests': 1,
        'ctest_test_names': ['objc3_a', 'objc3_b'], 'selected_ctest_names': ['objc3_b'],
        'tests': [{'name': 'objc3_b'}]}))
    (frontend / 'frontend-objc3_b-ctest.xml').write_text(
        '<testsuite tests="1"><testcase name="objc3_b"/></testsuite>')
    assert suite_results(tmp_path, 'frontend', frontend_names=frozenset({'objc3_b'})) == [{'name': 'objc3_b'}]
    for selection in (None, frozenset({'objc3_a'})):
        with pytest.raises(ValueError, match='executed tests are incomplete'):
            suite_results(tmp_path, 'frontend', frontend_names=selection)


@pytest.mark.parametrize('defect', [None, 'path', 'length', 'seal', 'source', 'target', 'channel'])
def test_download_pins_release_archive_and_cleans_temporary_bytes(tmp_path, monkeypatch, defect):
    host, tag = preview_download.native_linux_host(), 'v0.1.0-preview.1'
    name = f'objc3-sdk-{tag}-{host}.tar.zst'
    archive = {'file': name, 'sha256': 'a'*64, 'bytes': 15,
               'target': {'linux-x86_64': 'x86_64', 'linux-aarch64': 'aarch64'}[host] + '-unknown-linux-gnu',
               'seal_sha256': 'b'*64, 'source_build_id': 'pinned-source'}
    if defect == 'path':
        archive['file'] = '../' + name
    if defect == 'length':
        archive['bytes'] += 1
    if defect == 'target':
        archive['target'] = 'arm64-apple-macos15.0'
    manifest = tmp_path / 'manifest.json'
    manifest.write_text(json.dumps({'tag': tag, 'schema': 'objc3c.linux-release.v1',
        'channel': 'experimental' if defect == 'channel' else 'preview', 'revision': 'a' * 40,
        'archives': {host: archive}}))
    destination = tmp_path / 'install'

    def download(command, *, check):
        assert check
        assert command[-1] == f'{preview_download.DOWNLOADS}/{tag}/{name}'
        Path(command[-2]).write_bytes(b'released bytes\n')

    def install(path, digest, root, *, expected_seal_sha256, expected_source_build_id):
        assert path.name == name
        assert digest == archive['sha256']
        assert root == destination / 'sdk'
        assert expected_seal_sha256 == archive['seal_sha256']
        assert expected_source_build_id == archive['source_build_id']
        return {'seal_sha256': 'c'*64 if defect == 'seal' else archive['seal_sha256'],
                'cost': {'source_build_id': 'wrong-source' if defect == 'source' else 'pinned-source'}}

    monkeypatch.setattr(preview_download.subprocess, 'run', download)
    monkeypatch.setattr(preview_download, 'install', install)
    if defect:
        with pytest.raises(ValueError, match='preview manifest|archive coordinates|archive length|differs from the release'):
            preview_download.install_preview(manifest, destination)
    else:
        assert preview_download.install_preview(manifest, destination)['seal_sha256'] == archive['seal_sha256']
    assert not list(destination.glob('preview-download-*'))


def test_preview_cache_reuses_verified_archive_and_replaces_same_length_corruption(tmp_path, monkeypatch):
    data = b'released bytes\n'
    record = {'file': 'sdk.tar.zst', 'bytes': len(data), 'sha256': hashlib.sha256(data).hexdigest()}
    transfers = []

    def curl(command, *, check):
        assert check and command[command.index('--max-filesize') + 1] == str(record['bytes'])
        Path(command[-2]).write_bytes(data)
        transfers.append(command[-1])

    monkeypatch.setattr(preview_download.subprocess, 'run', curl)
    cache = tmp_path / 'cache'
    archive, first_source = preview_download.acquire_archive(tmp_path, 'v0.1.0-preview.1', record, cache)
    assert first_source == 'download'
    assert archive == cache / (record['sha256'] + '.tar.zst')
    assert preview_download.acquire_archive(tmp_path, 'v0.1.0-preview.1', record, cache) == (archive, 'cache')
    assert len(transfers) == 1

    archive.write_bytes(b'0' * len(data))
    assert preview_download.acquire_archive(tmp_path, 'v0.1.0-preview.1', record, cache) == (archive, 'download')
    assert archive.read_bytes() == data and len(transfers) == 2
    assert not list(cache.glob('.preview-download-*'))


@pytest.mark.parametrize('interrupted', [False, True])
def test_preview_cache_does_not_publish_failed_transfer_or_invalid_bytes(tmp_path, monkeypatch, interrupted):
    data = b'expected bytes\n'
    record = {'file': 'sdk.tar.zst', 'bytes': len(data), 'sha256': hashlib.sha256(data).hexdigest()}

    def curl(command, *, check):
        assert check
        Path(command[-2]).write_bytes(b'0' * len(data))
        if interrupted:
            raise subprocess.CalledProcessError(18, command)

    monkeypatch.setattr(preview_download.subprocess, 'run', curl)
    cache = tmp_path / 'cache'
    failure = subprocess.CalledProcessError if interrupted else ValueError
    with pytest.raises(failure, match=None if interrupted else 'SHA-256 differs'):
        preview_download.acquire_archive(tmp_path, 'v0.1.0-preview.1', record, cache)
    assert not list(cache.glob('*.tar.zst'))
    assert not list(cache.glob('.preview-download-*'))


def test_concurrent_preview_cache_acquisition_downloads_once(tmp_path, monkeypatch):
    data = b'one verified preview archive'
    record = {'file': 'sdk.tar.zst', 'bytes': len(data), 'sha256': hashlib.sha256(data).hexdigest()}
    transfers = []
    start = Barrier(2)
    cache = tmp_path / 'cache'

    def curl(command, *, check):
        assert check
        transfers.append(command[-1])
        Path(command[-2]).write_bytes(data)

    def acquire(_):
        start.wait(timeout=5)
        return preview_download.acquire_archive(tmp_path, 'v0.1.0-preview.1', record, cache)

    monkeypatch.setattr(preview_download.subprocess, 'run', curl)
    with ThreadPoolExecutor(max_workers=2) as pool:
        results = list(pool.map(acquire, range(2)))
    assert {source for _, source in results} == {'cache', 'download'}
    assert len(transfers) == 1
    assert all(path.read_bytes() == data for path, _ in results)
