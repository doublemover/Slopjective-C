"""Manage one active experimental Apple Silicon SDK under a dedicated root."""
from __future__ import annotations

from contextlib import contextmanager
import fcntl
import json
import os
from pathlib import Path
import re
import shutil
import stat
import tempfile

from . import darwin_sdk as sdk
from .package import MANIFEST, _file_hash, verify_sdk


SCHEMA = 'objc3c.experimental-darwin-sdk-activation.v1'
ADMISSION = 'admission.json'
GENERATION = re.compile(r'sdk-([a-f0-9]{64})')


@contextmanager
def _locked_root(root: Path, *, create: bool):
    root = root.expanduser().absolute()
    if root == root.parent or root.is_symlink():
        raise ValueError('Darwin SDK management requires a dedicated real root directory')
    root = root.parent.resolve(strict=True) / root.name
    if create:
        root.mkdir(mode=0o755, exist_ok=True)
    if not root.is_dir() or root.is_symlink():
        raise ValueError('Darwin SDK management requires a dedicated real root directory')
    flags = os.O_RDWR | os.O_NOFOLLOW | os.O_NONBLOCK
    try:
        descriptor = os.open(root / '.objc3-sdk.lock', flags | (os.O_CREAT if create else 0), 0o600)
    except FileNotFoundError as error:
        raise ValueError('Darwin SDK root has no managed installation') from error
    with os.fdopen(descriptor, 'r+b') as lock:
        if not stat.S_ISREG(os.fstat(lock.fileno()).st_mode):
            raise ValueError('Darwin SDK management lock must be a regular file')
        try:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError as error:
            raise ValueError('another Darwin SDK operation holds the installation root') from error
        yield root


def _verify_generation(generation: Path, host: dict | None = None) -> dict:
    seal = GENERATION.fullmatch(generation.name)
    prefix = generation / 'sdk'
    receipt = generation / ADMISSION
    if (not seal or generation.is_symlink() or not generation.is_dir() or
            {entry.name for entry in generation.iterdir()} != {'sdk', ADMISSION} or
            prefix.is_symlink() or not prefix.is_dir() or receipt.is_symlink() or
            not receipt.is_file() or receipt.stat().st_size > 64 * 1024):
        raise ValueError(f'Darwin SDK generation is outside managed ownership: {generation}')
    data = json.loads(receipt.read_bytes())
    if not isinstance(data, dict):
        raise ValueError(f'Darwin SDK generation admission changed: {generation}')
    archive = data.get('archive_sha256')
    previous = data.get('previous_seal_sha256')
    if (data.get('schema') != SCHEMA or data.get('seal_sha256') != seal[1] or
            not isinstance(archive, str) or not sdk.HEX_SHA.fullmatch(archive) or
            (previous is not None and (not isinstance(previous, str) or
                                       not sdk.HEX_SHA.fullmatch(previous) or previous == seal[1]))):
        raise ValueError(f'Darwin SDK generation admission changed: {generation}')
    if ((prefix / MANIFEST).is_symlink() or
            any(entry.is_symlink() or not (entry.is_file() or entry.is_dir()) for entry in prefix.rglob('*'))):
        raise ValueError(f'Darwin SDK generation contains unowned files: {generation}')
    manifest = verify_sdk(prefix, seal[1])
    sdk._validate_manifest(manifest, data['host'])
    if host is not None:
        sdk._validate_manifest(manifest, host)
    return data


def _active(root: Path) -> Path | None:
    current = root / 'current'
    if not current.is_symlink():
        if current.exists():
            raise ValueError('Darwin SDK current path must be the owned symbolic link')
        return None
    value = os.readlink(current)
    target = Path(value)
    if (len(target.parts) != 2 or not GENERATION.fullmatch(target.parts[0]) or
            target.parts[1] != 'sdk' or value != target.as_posix()):
        raise ValueError('Darwin SDK current points outside the managed installation')
    return root / target.parts[0]


def _reclaim(root: Path, seal: str | None) -> None:
    if seal is None:
        return
    previous = root / ('sdk-' + seal)
    if previous.exists() or previous.is_symlink():
        _verify_generation(previous)
        shutil.rmtree(previous)


def _result(root: Path, action: str, admission: dict, cleanup_error: Exception | None = None) -> dict:
    receipt = {'schema': SCHEMA, 'status': 'PASS', 'action': action, 'support_truth': False,
               'root': str(root), 'active_prefix': None if action == 'uninstalled' else str(root / 'current'),
               'seal_sha256': admission['seal_sha256'], 'archive_sha256': admission['archive_sha256']}
    if cleanup_error is not None:
        receipt['cleanup_error'] = str(cleanup_error)
    return receipt


def install(archive: Path, expected_sha256: str, root: Path) -> dict:
    host = sdk._host()
    if not sdk.HEX_SHA.fullmatch(expected_sha256):
        raise ValueError('installation requires the lowercase package archive SHA-256')
    archive = archive.resolve(strict=True)
    if not archive.is_file() or _file_hash(archive) != expected_sha256:
        raise ValueError('Darwin package archive SHA-256 does not match the receipt')
    with _locked_root(root, create=True) as root:
        active = _active(root)
        old = _verify_generation(active, host) if active else None
        if old:
            _reclaim(root, old.get('previous_seal_sha256'))
            if old['archive_sha256'] == expected_sha256:
                return _result(root, 'reused', old)

        with tempfile.TemporaryDirectory(prefix='.incoming-', dir=root) as directory:
            staging = Path(directory)
            candidate = staging / 'candidate'
            candidate.mkdir()
            verified = sdk.install(archive, expected_sha256, candidate / 'sdk')
            seal = verified['seal_sha256']
            if old and old['seal_sha256'] == seal:
                admission = {**old, 'archive_sha256': expected_sha256, 'host': verified['host']}
                receipt = staging / ADMISSION
                receipt.write_text(json.dumps(admission, indent=2) + '\n')
                os.replace(receipt, active / ADMISSION)
                return _result(root, 'reused', admission)
            admission = {'schema': SCHEMA, 'seal_sha256': seal, 'archive_sha256': expected_sha256,
                         'host': verified['host'], 'previous_seal_sha256': old['seal_sha256'] if old else None}
            destination = root / ('sdk-' + seal)
            (candidate / ADMISSION).write_text(json.dumps(admission, indent=2) + '\n')
            pending = staging / 'current'
            pending.symlink_to((destination / 'sdk').relative_to(root))
            existing = destination.exists() or destination.is_symlink()
            if existing:
                if _verify_generation(destination, host) != admission:
                    raise ValueError(f'Darwin SDK generation path belongs to another admission: {destination}')
            else:
                candidate.rename(destination)
            try:
                os.replace(pending, root / 'current')
            except BaseException:
                if not existing and _active(root) != destination:
                    shutil.rmtree(destination)
                raise

        cleanup_error = None
        try:
            _reclaim(root, admission['previous_seal_sha256'])
        except (OSError, ValueError, KeyError) as error:
            cleanup_error = error
        return _result(root, 'updated' if old else 'installed', admission, cleanup_error)


def uninstall(root: Path, expected_seal: str) -> dict:
    sdk.require_apple_silicon()
    if not sdk.HEX_SHA.fullmatch(expected_seal):
        raise ValueError('uninstall requires the lowercase installed SDK seal SHA-256')
    with _locked_root(root, create=False) as root:
        active = _active(root)
        if active is not None and active.name != 'sdk-' + expected_seal:
            raise ValueError('the requested SDK seal differs from the active Darwin SDK')
        candidate = root / ('sdk-' + expected_seal)
        admission = _verify_generation(candidate)
        if active is not None:
            (root / 'current').unlink()
        shutil.rmtree(candidate)
        return _result(root, 'uninstalled', admission)
