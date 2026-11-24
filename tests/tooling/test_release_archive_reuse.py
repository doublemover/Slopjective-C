"""A published archive outlives the mutable build candidate that produced it."""
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tarfile

import pytest

from scripts.objc3c_toolchain import linux_archive_reuse, linux_sdk, package
from scripts.objc3c_toolchain.linux_host import native_linux_host, native_target
from scripts.objc3c_toolchain.install_sdk import install


IDENTITY = hashlib.sha256(b'current-inputs').hexdigest()


def cached_release(tmp_path, monkeypatch):
    repo = tmp_path / 'repo'
    foundation = tmp_path / 'foundation'
    (foundation / 'bin').mkdir(parents=True)
    (foundation / 'bin/objc3c').write_text('qualified compiler')
    (foundation / 'bin/objc3c').chmod(0o755)
    verifier = foundation / 'share/objc3c/verify-sdk.py'
    verifier.parent.mkdir(parents=True)
    shutil.copyfile(package.__file__, verifier)
    package.normalize_permissions(foundation)
    recipe = {'upstream_commit': 'test-upstream', 'patch_digest': 'test-patches',
              'build_config_digest': 'test-configuration'}
    data = {**recipe, 'schema': package.SCHEMA, 'source_build_id': 'test-release', 'target': native_target(),
            'linux_abi': {'glibc_minimum': '2.17', 'libstdcxx_symbol_minimum': 'GLIBCXX_3.4'},
            'files': package.inventory(foundation)}
    data['seal_sha256'] = package._digest(data)
    (foundation / package.MANIFEST).write_text(json.dumps(data))
    (foundation / package.MANIFEST).chmod(0o644)
    archive = repo / 'out/toolchains/sdk-cache/sdk.tar.zst'
    archive.parent.mkdir(parents=True)
    published = package._archive_sdk(foundation, archive, {
        'archive': str(archive), 'seal_sha256': data['seal_sha256'], 'source_build_id': 'test-release',
        'host': native_linux_host(), 'target': native_target()})
    directory = repo / 'out/toolchains/linux-sdk-builds' / IDENTITY
    directory.mkdir(parents=True)
    receipt = {**published, 'status': 'PASS', 'build_prefix': str(directory / 'prefix'), 'input_identity': IDENTITY}
    (directory / 'release.json').write_text(json.dumps(receipt))
    monkeypatch.setenv('OBJC3C_BOOTSTRAP_LLVM_ROOT', str(foundation))
    monkeypatch.setattr(linux_sdk, 'verify_development', lambda _: {**recipe, 'build_id': 'test-release'})
    monkeypatch.setattr(linux_sdk, 'input_identity', lambda *args: {'sha256': IDENTITY})
    return repo, foundation, receipt


def test_archived_release_is_reused_after_candidate_is_removed(tmp_path, monkeypatch):
    repo, foundation, receipt = cached_release(tmp_path, monkeypatch)
    assert not Path(receipt['build_prefix']).exists()
    result = linux_sdk.build_sdk(repo, foundation_sdk=foundation)
    assert result['cache_hit']
    assert result['installed']['seal_sha256'] == receipt['seal_sha256']
    assert (repo / 'out/toolchains/linux-sdk/current').resolve() == Path(result['installed']['prefix'])
    package.verify_sdk(Path(result['installed']['prefix']), receipt['seal_sha256'])


def test_published_archive_owns_reuse_even_if_the_candidate_changes(tmp_path, monkeypatch):
    repo, foundation, receipt = cached_release(tmp_path, monkeypatch)
    candidate = Path(receipt['build_prefix'])
    shutil.copytree(foundation, candidate)
    (candidate / 'bin/objc3c').write_text('unpublished local edit')
    result = linux_sdk.build_sdk(repo, foundation_sdk=foundation)
    assert result['cache_hit']
    assert (Path(result['installed']['prefix']) / 'bin/objc3c').read_text() == 'qualified compiler'


def test_corrupted_cached_archive_preserves_the_active_installation(tmp_path, monkeypatch):
    repo, foundation, receipt = cached_release(tmp_path, monkeypatch)
    root = repo / 'out/toolchains/linux-sdk'
    installed = install(Path(receipt['archive']), receipt['archive_sha256'], root)
    with Path(receipt['archive']).open('ab') as stream:
        stream.write(b'corrupted after publication')
    with pytest.raises(ValueError, match='SHA-256'):
        linux_sdk.build_sdk(repo, foundation_sdk=foundation)
    assert (root / 'current').resolve() == Path(installed['prefix'])
    package.verify_sdk(Path(installed['prefix']), receipt['seal_sha256'])


def test_exact_replay_requires_existing_archive_admission(tmp_path, monkeypatch):
    repo, foundation, _ = cached_release(tmp_path, monkeypatch)
    with pytest.raises(ValueError, match='existing sealed prefix and archive admission'):
        linux_sdk.build_sdk(repo, foundation_sdk=foundation, reuse_input_identity=IDENTITY)
    assert not (repo / 'out/toolchains/linux-sdk/current').exists()


def test_same_run_replay_records_measured_readmission_without_build_or_extraction(tmp_path, monkeypatch):
    repo, _, receipt = cached_release(tmp_path, monkeypatch)
    root = repo / 'out/toolchains/linux-sdk'
    receipt['installed'] = install(Path(receipt['archive']), receipt['archive_sha256'], root)
    canonical = Path(receipt['build_prefix']).with_name('release.json')
    canonical.write_text(json.dumps(receipt))
    producer_receipt = repo / 'producer.json'
    producer_receipt.write_text(json.dumps(receipt))
    subprocess.run(['git', 'init', '-q'], cwd=repo, check=True)
    subprocess.run(['git', '-c', 'user.name=Archive Replay', '-c', 'user.email=replay@example.com',
                    'commit', '-qm', 'Producer revision', '--allow-empty'], cwd=repo, check=True)

    def reject_work(*args, **kwargs):
        pytest.fail('warm replay entered archive production, extraction or compilation')

    for name in ('build_foundation', 'copy_foundation', 'package_sdk', '_archive_sdk'):
        monkeypatch.setattr(linux_sdk, name, reject_work)
    monkeypatch.setattr(tarfile.TarFile, 'extractall', reject_work)
    output = repo / 'replay.json'
    assert linux_archive_reuse.main(['--repo', str(repo), '--producer-receipt', str(producer_receipt),
                                     '--output', str(output)]) == 0
    replay = json.loads(output.read_text())
    assert replay['status'] == 'PASS'
    assert replay['input_identity'] == IDENTITY
    assert replay['before'] == replay['after']
    assert replay['after']['archive'] == {'sha256': receipt['archive_sha256'], 'bytes': receipt['archive_bytes']}
    cost = replay['installation']['installed']['cost']
    assert cost['prefix_reused'] and cost['seal_sha256'] == receipt['seal_sha256']
    assert replay['stale_request']['status'] == 'REJECTED'
    assert replay['stale_request']['requested_input_identity'] != IDENTITY
    assert set(replay['phases_seconds']) == {'foundation_admission', 'qualification_identity',
                                            'receipt_and_archive_readmission'}
    assert 0 < sum(replay['phases_seconds'].values()) <= replay['replay_elapsed_seconds']

    sidecar = Path(receipt['archive']).with_suffix('.json')
    sidecar.write_text(json.dumps({**json.loads(sidecar.read_text()), 'seal_sha256': 'other'}))
    with pytest.raises(ValueError, match='package receipt differs'):
        linux_archive_reuse.qualify(repo, producer_receipt)
    assert (root / 'current').resolve() == Path(receipt['installed']['prefix'])
