"""A published archive outlives the mutable build candidate that produced it."""
from compression import zstd
import json
from pathlib import Path
import shutil
import tarfile

import pytest

from scripts.objc3c_toolchain import linux_sdk, package
from scripts.objc3c_toolchain.install_sdk import file_hash, install


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
    data = {'schema': package.SCHEMA, 'source_build_id': 'test-release',
            'files': package.inventory(foundation)}
    data['seal_sha256'] = package._digest(data)
    (foundation / package.MANIFEST).write_text(json.dumps(data))
    (foundation / package.MANIFEST).chmod(0o644)
    archive = tmp_path / 'sdk.tar.zst'
    with zstd.ZstdFile(archive, 'wb') as stream:
        with tarfile.open(fileobj=stream, mode='w|') as tar:
            tar.add(foundation, arcname='objc3-sdk')
    directory = repo / 'out/toolchains/linux-sdk-builds/current-inputs'
    directory.mkdir(parents=True)
    receipt = {'status': 'PASS', 'build_prefix': str(directory / 'prefix'),
               'archive': str(archive), 'archive_sha256': file_hash(archive),
               'seal_sha256': data['seal_sha256'], 'source_build_id': 'test-release'}
    (directory / 'release.json').write_text(json.dumps(receipt))
    monkeypatch.setenv('OBJC3C_BOOTSTRAP_LLVM_ROOT', str(foundation))
    monkeypatch.setattr(linux_sdk, 'verify_development', lambda _: {'build_id': 'test-release'})
    monkeypatch.setattr(linux_sdk, 'input_identity', lambda *args: {'sha256': 'current-inputs'})
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
