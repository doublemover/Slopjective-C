import json
from pathlib import Path
import subprocess

import pytest

from scripts.objc3c_assurance import pr_native, preview_download


@pytest.fixture
def checkout(tmp_path):
    def git(*arguments):
        return subprocess.check_output(['git', *arguments], cwd=tmp_path, text=True).strip()

    git('init', '-q')
    git('config', 'user.name', 'Native PR Test')
    git('config', 'user.email', 'native-pr@example.org')
    git('commit', '-qm', 'base', '--allow-empty')
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

    def execute(repo, plan, output, *, suite_ids, product_sdk):
        assert plan['revision'] == revision
        assert plan['changed_paths'] == [changed]
        assert not output.exists()
        assert product_sdk == (sdk if expected == pr_native.SOURCE_SUITES else None)
        executed.extend(suite_ids)
        return {'status': 'PASS', 'revision': plan['revision'], 'plan_sha256': plan['sha256'],
                'suites': [{'id': name, 'status': 'PASS', 'executions': ['witness']}
                           for name in suite_ids]}

    monkeypatch.setattr(pr_native, 'run', execute)
    output = repo / 'out' / 'native-pr'
    report = pr_native.execute(repo, output, **revision, sdk=sdk)
    assert report['suites'] == executed == list(expected)
    assert report['status'] == 'PASS'
    assert report['revision'] == revision
    assert json.loads((output / 'result.json').read_text()) == report
    with pytest.raises(ValueError, match='fresh owned output'):
        pr_native.execute(repo, output, **revision, sdk=sdk)


@pytest.mark.parametrize('defect', ['revision', 'digest', 'missing', 'empty', 'failure', 'seal'])
def test_pr_gate_records_failure_for_incomplete_or_drifting_evidence(checkout, monkeypatch, defect):
    repo, commit = checkout
    revision = commit('tests/clang_foundation/runtime.m')
    foundations = iter([{'seal_sha256': 'a'*64}, {'seal_sha256': 'b'*64}])
    monkeypatch.setattr(pr_native, 'matching_foundation', lambda *arguments: next(foundations))

    def execute(repo, plan, output, *, suite_ids, product_sdk):
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


@pytest.mark.parametrize('defect', [None, 'path', 'length', 'seal', 'source'])
def test_download_pins_release_archive_and_cleans_temporary_bytes(tmp_path, monkeypatch, defect):
    host, tag = preview_download.native_linux_host(), 'v0.1.0-preview.1'
    name = f'objc3-sdk-{tag}-{host}.tar.zst'
    archive = {'file': name, 'sha256': 'a'*64, 'bytes': 15,
               'seal_sha256': 'b'*64, 'source_build_id': 'pinned-source'}
    if defect == 'path':
        archive['file'] = '../' + name
    if defect == 'length':
        archive['bytes'] += 1
    manifest = tmp_path / 'manifest.json'
    manifest.write_text(json.dumps({'tag': tag, 'archives': {host: archive}}))
    destination = tmp_path / 'install'

    def download(command, *, check):
        assert check
        assert command[-1] == f'{preview_download.DOWNLOADS}/{tag}/{name}'
        Path(command[-2]).write_bytes(b'released bytes\n')

    def install(path, digest, root):
        assert path.name == name
        assert digest == archive['sha256']
        assert root == destination / 'sdk'
        return {'seal_sha256': 'c'*64 if defect == 'seal' else archive['seal_sha256'],
                'cost': {'source_build_id': 'wrong-source' if defect == 'source' else 'pinned-source'}}

    monkeypatch.setattr(preview_download.subprocess, 'run', download)
    monkeypatch.setattr(preview_download, 'install', install)
    if defect:
        with pytest.raises(ValueError, match='archive coordinates|archive length|differs from the release'):
            preview_download.install_preview(manifest, destination)
    else:
        assert preview_download.install_preview(manifest, destination)['seal_sha256'] == archive['seal_sha256']
    assert not list(destination.glob('preview-download-*'))
