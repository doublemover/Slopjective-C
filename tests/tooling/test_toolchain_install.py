from __future__ import annotations
from compression import zstd
import hashlib
import json
from pathlib import Path
import shutil
import tarfile

import pytest

from scripts.objc3c_toolchain import package
from scripts.objc3c_toolchain.install_sdk import file_hash, install

pytestmark = pytest.mark.skipif(not shutil.which('zstd'), reason='installer needs zstd')


def release(tmp_path: Path):
    prefix = tmp_path / 'source'
    (prefix / 'bin').mkdir(parents=True)
    (prefix / 'bin/objc3c').write_text('compiler')
    (prefix / 'bin/objc3c').chmod(0o700)
    verifier = prefix / 'share/objc3c/verify-sdk.py'
    verifier.parent.mkdir(parents=True)
    shutil.copyfile(package.__file__, verifier)
    package.normalize_permissions(prefix)
    data = {'schema': package.SCHEMA, 'source_build_id': 'test-release', 'files': package.inventory(prefix)}
    data['seal_sha256'] = package._digest(data)
    (prefix / package.MANIFEST).write_text(json.dumps(data))
    (prefix / package.MANIFEST).chmod(0o644)
    archive = tmp_path / 'sdk.tar.zst'
    with zstd.ZstdFile(archive, 'wb') as stream:
        with tarfile.open(fileobj=stream, mode='w|') as tar:
            tar.add(prefix, arcname='objc3-sdk')
    return archive, file_hash(archive)


def test_install_reuse_and_relocation(tmp_path):
    archive, digest = release(tmp_path)
    root = tmp_path / 'installation with spaces'
    first = install(archive, digest, root)
    prefix = Path(first['prefix'])
    assert prefix.name == first['seal_sha256']
    assert (root / 'current').resolve() == prefix
    assert install(archive, digest, root)['prefix'] == first['prefix']
    moved = tmp_path / 'relocated'
    root.rename(moved)
    package.verify_sdk((moved / 'current').resolve(), first['seal_sha256'])


def test_bad_archive_and_tampered_prefix_preserve_activation(tmp_path):
    archive, digest = release(tmp_path)
    root = tmp_path / 'installed'
    installed = install(archive, digest, root)
    with pytest.raises(ValueError, match='SHA-256'):
        install(archive, '0' * 64, root)
    (Path(installed['prefix']) / 'bin/objc3c').write_text('tampered')
    with pytest.raises(ValueError, match='differ'):
        install(archive, digest, root)
    assert (root / 'current').resolve() == Path(installed['prefix'])


def test_cached_verifier_cannot_hide_modified_installation_contents(tmp_path):
    archive, digest = release(tmp_path)
    root = tmp_path / 'installed'
    installed = install(archive, digest, root)
    prefix = Path(installed['prefix'])
    (prefix / 'bin/objc3c').write_text('modified compiler')
    (prefix / 'share/objc3c/verify-sdk.py').write_text(
        "def verify_sdk(prefix):\n    return " + repr({'seal_sha256': installed['seal_sha256']}) + "\n")
    with pytest.raises(ValueError, match='differ'):
        install(archive, digest, root)
    assert (root / 'current').resolve() == prefix


def test_installer_rejects_archive_paths_escaping_destination(tmp_path):
    archive = tmp_path / 'bad.tar.zst'
    with zstd.ZstdFile(archive, 'wb') as stream:
        with tarfile.open(fileobj=stream, mode='w|') as tar:
            info = tarfile.TarInfo('../../outside')
            tar.addfile(info)
    with pytest.raises(tarfile.FilterError):
        install(archive, file_hash(archive), tmp_path / 'installed')
    assert not (tmp_path / 'outside').exists()


def test_standalone_installer_does_not_write_bytecode_into_sealed_prefix(tmp_path):
    import os
    import subprocess
    import sys
    from scripts.objc3c_toolchain import install_sdk
    archive, digest = release(tmp_path)
    result = subprocess.run([sys.executable, install_sdk.__file__, '--archive', str(archive),
                             '--sha256', digest, '--root', str(tmp_path / 'installed')],
                            env={key: value for key, value in os.environ.items()
                                 if key != 'PYTHONDONTWRITEBYTECODE'}, capture_output=True, text=True)
    assert result.returncode == 0, result.stderr
    prefix = Path(json.loads(result.stdout)['prefix'])
    assert not list(prefix.rglob('*.pyc'))
    package.verify_sdk(prefix)


def test_root_install_is_readable_and_executable_with_a_private_umask(tmp_path):
    import os
    archive, digest = release(tmp_path)
    previous = os.umask(0o077)
    try:
        root = tmp_path / 'shared-installation'
        installed = install(archive, digest, root)
    finally:
        os.umask(previous)
    assert root.stat().st_mode & 0o777 == 0o755
    assert (root / 'sdks').stat().st_mode & 0o777 == 0o755
    assert (root / 'activate.sh').stat().st_mode & 0o777 == 0o644
    prefix = Path(installed['prefix'])
    assert prefix.stat().st_mode & 0o777 == 0o755
    assert (prefix / 'bin/objc3c').stat().st_mode & 0o777 == 0o755
    package.verify_sdk(prefix)
