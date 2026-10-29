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


def selected_by_shell(root):
    import subprocess
    result = subprocess.run(['sh', '-c', '. "$1/activate.sh" && printf "%s" "$OBJC3C_SDK_ROOT"',
                             'sh', str(root)], capture_output=True, text=True)
    return Path(result.stdout) if result.returncode == 0 else None


@pytest.mark.parametrize('old_generation', [False, True])
@pytest.mark.parametrize('boundary', ['write', 'chmod', 'reader-rename', 'symlink', 'commit'])
def test_activation_precommit_failure_preserves_coherent_selection(tmp_path, monkeypatch, old_generation, boundary):
    import errno
    from scripts.objc3c_toolchain import install_sdk
    root = tmp_path / "install 'with spaces"
    a, b = root / 'sdks/A', root / 'sdks/B'
    for path in (a, b):
        path.mkdir(parents=True)
    if old_generation:
        install_sdk.activate(root, a)
        # Exercise migration from an older concrete-path activation reader.
        (root / 'activate.sh').write_text('export OBJC3C_SDK_ROOT=' + __import__('shlex').quote(str(a)) + '\n')
    write, chmod, replace, symlink = Path.write_text, Path.chmod, install_sdk.os.replace, Path.symlink_to
    def fail():
        raise OSError(errno.ENOSPC, 'injected activation failure')
    def wrapped_write(path, *args, **kw):
        if boundary == 'write' and path.name.startswith('.activate-'):
            fail()
        return write(path, *args, **kw)
    def wrapped_chmod(path, *args, **kw):
        if boundary == 'chmod' and path.name.startswith('.activate-'):
            fail()
        return chmod(path, *args, **kw)
    def wrapped_replace(src, dst):
        if ((boundary == 'reader-rename' and Path(dst).name == 'activate.sh') or
                (boundary == 'commit' and Path(dst).name == 'current')):
            fail()
        return replace(src, dst)
    def wrapped_symlink(path, *args, **kw):
        if boundary == 'symlink' and path.name.startswith('.current-'):
            fail()
        return symlink(path, *args, **kw)
    with monkeypatch.context() as patch:
        patch.setattr(Path, 'write_text', wrapped_write)
        patch.setattr(Path, 'chmod', wrapped_chmod)
        patch.setattr(install_sdk.os, 'replace', wrapped_replace)
        patch.setattr(Path, 'symlink_to', wrapped_symlink)
        with pytest.raises(OSError, match='injected activation failure'):
            install_sdk.activate(root, b)
    expected = a if old_generation else None
    assert ((root / 'current').resolve() if (root / 'current').exists() else None) == expected
    assert selected_by_shell(root) == expected
    # A failed attempt remains idempotently retryable; only current publishes B.
    install_sdk.activate(root, b)
    assert selected_by_shell(root) == (root / 'current').resolve() == b


@pytest.mark.parametrize('old_generation', [False, True])
@pytest.mark.parametrize('commit', [False, True])
def test_process_termination_at_activation_commit_is_coherent(tmp_path, old_generation, commit):
    import os
    import subprocess
    import sys
    from scripts.objc3c_toolchain import install_sdk
    root = tmp_path / "install 'with spaces"
    a, b = root / 'sdks/A', root / 'sdks/B'
    for path in (a, b):
        path.mkdir(parents=True)
    if old_generation:
        install_sdk.activate(root, a)
    code = '''
import os, sys
from pathlib import Path
from scripts.objc3c_toolchain import install_sdk
root = Path(sys.argv[1])
replace = os.replace
def stop(src, dst):
    if Path(dst).name == 'current':
        if sys.argv[2] == 'True':
            replace(src, dst)
        os._exit(99)
    return replace(src, dst)
os.replace = stop
install_sdk.activate(root, root / 'sdks/B')
'''
    env = {**os.environ, 'PYTHONPATH': str(Path(__file__).resolve().parents[2])}
    completed = subprocess.run([sys.executable, '-c', code, str(root), str(commit)], env=env)
    assert completed.returncode == 99
    expected = b if commit else (a if old_generation else None)
    assert ((root / 'current').resolve() if (root / 'current').exists() else None) == expected
    assert selected_by_shell(root) == expected
    install_sdk.activate(root, b)
    assert selected_by_shell(root) == b


def test_activation_reader_is_stable_and_existing_shell_remains_pinned(tmp_path):
    import subprocess
    from scripts.objc3c_toolchain.install_sdk import activate
    root = tmp_path / 'installed'
    a, b = root / 'sdks/A', root / 'sdks/B'
    for path in (a, b):
        path.mkdir(parents=True)
    activate(root, a)
    script = (root / 'activate.sh').read_bytes()
    activate(root, b)
    assert (root / 'activate.sh').read_bytes() == script
    assert selected_by_shell(root) == b
    activate(root, a)
    assert selected_by_shell(root) == a
    moved = tmp_path / 'relocated'
    root.rename(moved)
    activate(moved, moved / 'sdks/B')
    assert selected_by_shell(moved) == moved / 'sdks/B'
