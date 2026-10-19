import hashlib
import json
from pathlib import Path

import pytest

from scripts.objc3c_toolchain import linux_sdk


@pytest.fixture
def pending(tmp_path):
    directory = tmp_path / 'build'
    prefix = directory / 'prefix'
    prefix.mkdir(parents=True)
    (prefix / 'qualified-product').write_bytes(b'completed SDK candidate')
    archive = tmp_path / 'sdk.tar.zst'
    archive.write_bytes(b'verified qualified archive')
    report = {'status': 'awaiting-installation', 'input_identity': 'input-key',
              'build_prefix': str(prefix), 'archive': str(archive),
              'archive_sha256': hashlib.sha256(archive.read_bytes()).hexdigest()}
    return directory, report


def test_reclaims_candidate_before_installing_archive(pending, tmp_path, monkeypatch):
    directory, report = pending
    def install(archive, expected, root):
        assert not (directory / 'prefix').exists()
        saved = json.loads((directory / 'qualified-archive.json').read_text())
        assert saved['status'] == 'awaiting-installation'
        assert saved['archive_sha256'] == expected
        return {'prefix': str(root / 'sealed-sdk')}
    monkeypatch.setattr(linux_sdk, 'install', install)
    installed = linux_sdk._install_qualified_archive(report, directory, tmp_path / 'installed', expected_identity='input-key')
    assert installed['build_candidate_reclaimed'] is True
    assert installed['installed']['prefix'].endswith('/installed/sealed-sdk')


@pytest.mark.parametrize('failure', ['archive', 'identity', 'prefix-path', 'prefix-link'])
def test_invalid_publication_preserves_candidate(pending, tmp_path, monkeypatch, failure):
    directory, report = pending
    prefix = directory / 'prefix'
    expected = 'input-key'
    if failure == 'archive':
        Path(report['archive']).write_bytes(b'damaged')
    elif failure == 'identity':
        expected = 'other-input-key'
    elif failure == 'prefix-path':
        report['build_prefix'] = str(tmp_path / 'unowned')
    else:
        target = tmp_path / 'external'
        prefix.rename(target)
        prefix.symlink_to(target, target_is_directory=True)
    def unexpected(*args):
        raise AssertionError('invalid publication reached installation')
    monkeypatch.setattr(linux_sdk, 'install', unexpected)
    with pytest.raises(ValueError):
        linux_sdk._install_qualified_archive(report, directory, tmp_path / 'installed', expected_identity=expected)
    assert (prefix / 'qualified-product').read_bytes() == b'completed SDK candidate'
    assert not (directory / 'qualified-archive.json').exists()


def test_failed_extraction_can_resume_without_candidate_or_requalification(pending, tmp_path, monkeypatch):
    directory, report = pending
    attempts = []
    def install(archive, expected, root):
        attempts.append(archive)
        if len(attempts) == 1:
            raise OSError('no space left during extraction')
        return {'prefix': str(root / 'sealed-sdk')}
    monkeypatch.setattr(linux_sdk, 'install', install)
    with pytest.raises(OSError, match='no space'):
        linux_sdk._install_qualified_archive(report, directory, tmp_path / 'installed', expected_identity='input-key')
    assert not (directory / 'prefix').exists()
    saved = json.loads((directory / 'qualified-archive.json').read_text())
    result = linux_sdk._install_qualified_archive(saved, directory, tmp_path / 'installed', expected_identity='input-key')
    assert result['build_candidate_reclaimed']
    assert len(attempts) == 2


def test_partial_reclamation_can_resume_from_verified_archive(pending, tmp_path, monkeypatch):
    directory, report = pending
    original = linux_sdk.shutil.rmtree
    def interrupted(path):
        (path / 'qualified-product').unlink()
        raise OSError('interrupted reclamation')
    monkeypatch.setattr(linux_sdk.shutil, 'rmtree', interrupted)
    with pytest.raises(OSError, match='interrupted reclamation'):
        linux_sdk._install_qualified_archive(report, directory, tmp_path / 'installed', expected_identity='input-key')
    saved = json.loads((directory / 'qualified-archive.json').read_text())
    monkeypatch.setattr(linux_sdk.shutil, 'rmtree', original)
    monkeypatch.setattr(linux_sdk, 'install', lambda *args: {'prefix': 'installed'})
    assert linux_sdk._install_qualified_archive(saved, directory, tmp_path / 'installed', expected_identity='input-key')['installed'] == {'prefix': 'installed'}
