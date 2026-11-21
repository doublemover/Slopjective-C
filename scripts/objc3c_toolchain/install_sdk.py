"""Install a verified Linux SDK into a content-addressed prefix and activate it."""
from __future__ import annotations

import argparse
import fcntl
import hashlib
import json
import os
from pathlib import Path
import platform
import shlex
import shutil
import subprocess
import tarfile
import tempfile
import time
import types
import uuid


def file_hash(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def admit_native_sdk(prefix: Path, data: dict) -> None:
    # Keep the downloaded installer self-contained: no checkout or product
    # module may be imported before its archive/seal has been admitted.
    machines = {'x86_64': ('x86_64-unknown-linux-gnu', 62),
                'amd64': ('x86_64-unknown-linux-gnu', 62),
                'aarch64': ('aarch64-unknown-linux-gnu', 183),
                'arm64': ('aarch64-unknown-linux-gnu', 183)}
    selected = machines.get(platform.machine().lower())
    if platform.system() != 'Linux' or selected is None or data.get('target') != selected[0]:
        raise ValueError('SDK target differs from the native Linux architecture')
    for name, row in data['files'].items():
        if row['kind'] != 'file':
            continue
        with (prefix / name).open('rb') as stream:
            header = stream.read(64)
        if header[:4] == b'\x7fELF' and (len(header) != 64 or
                header[:6] != b'\x7fELF\x02\x01' or
                int.from_bytes(header[18:20], 'little') != selected[1]):
            raise ValueError('SDK ELF member differs from the native Linux architecture: ' + name)


def verify(prefix: Path, *, verifier: Path | None = None, verifier_bytes: bytes | None = None) -> dict:
    # Loading a module through importlib writes __pycache__ unless the parent
    # happens to use -B. A verifier must never mutate the sealed installation.
    path = verifier or prefix / 'share/objc3c/verify-sdk.py'
    module = types.ModuleType('objc3_sdk_verifier')
    module.__file__ = str(path)
    exec(compile(path.read_bytes() if verifier_bytes is None else verifier_bytes, str(path), 'exec'),
         module.__dict__)
    return module.verify_sdk(prefix)


def existing_generation(root: Path, archive: Path, decoder_path: Path) -> tuple[Path, str, bytes] | None:
    generations = root / 'sdks'
    if not generations.is_dir() or generations.is_symlink():
        return None
    # New archives put the seal and standalone verifier before the bulk data.
    # Read them from the hash-verified archive, never from the mutable prefix.
    with subprocess.Popen([str(decoder_path), '-dc', str(archive)], stdout=subprocess.PIPE) as decoder:
        try:
            with tarfile.open(fileobj=decoder.stdout, mode='r|') as tar:
                admission = []
                for name, limit in (('objc3-sdk/objc3-sdk.json', 32 * 1024 * 1024),
                                    ('objc3-sdk/share/objc3c/verify-sdk.py', 256 * 1024)):
                    member = tar.next()
                    if member is None or member.name != name or not member.isfile() or member.size > limit:
                        return None
                    with tar.extractfile(member) as source:
                        admission.append(source.read())
        finally:
            decoder.terminate()
            decoder.stdout.close()
    seal = json.loads(admission[0])['seal_sha256']
    if len(seal) != 64 or any(character not in '0123456789abcdef' for character in seal):
        raise ValueError('SDK archive contains an invalid generation seal')
    prefix = generations / seal
    return (prefix, seal, admission[1]) if prefix.is_dir() and not prefix.is_symlink() else None


def create_install_directory(path: Path, *, parents: bool = False) -> None:
    try:
        path.mkdir(parents=parents)
    except FileExistsError:
        if not path.is_dir() or path.is_symlink():
            raise
    else:
        path.chmod(0o755)


def activate(root: Path, prefix: Path) -> None:
    active = root / 'current'
    if active.exists() and not active.is_symlink():
        raise ValueError(f'activation path is not a symbolic link; preserving {active}')
    script = root / 'activate.sh'
    text = '''# Source this file in a POSIX shell. Bootstrap and product SDKs are separate.
if [ -z "${OBJC3C_BOOTSTRAP_LLVM_ROOT:-}" ] && [ -n "${LLVM_ROOT:-}" ] && [ ! -f "$LLVM_ROOT/objc3-sdk.json" ]; then
  export OBJC3C_BOOTSTRAP_LLVM_ROOT="$LLVM_ROOT"
fi
# Resolve the single published pointer once. Existing shells keep this concrete
# generation even when a later installation changes current.
_objc3_selected=$(CDPATH= cd -P -- SDK_PATH && pwd -P) || return 1
# Drop selected and previously activated SDK bin entries before prepending.
_objc3_previous=${OBJC3C_SDK_ROOT:-}
_objc3_path=$_objc3_selected/bin
_objc3_rest=${PATH:-}
while case $_objc3_rest in
  *:*) _objc3_entry=${_objc3_rest%%:*}; _objc3_rest=${_objc3_rest#*:};;
  *) _objc3_entry=$_objc3_rest; unset _objc3_rest;;
esac
do
  if [ "$_objc3_entry" != "$_objc3_selected/bin" ] &&
     { [ -z "$_objc3_previous" ] || [ "$_objc3_entry" != "$_objc3_previous/bin" ]; }; then
    _objc3_path=$_objc3_path:$_objc3_entry
  fi
  [ "${_objc3_rest+x}" ] || break
done
export PATH="$_objc3_path"
export OBJC3C_SDK_ROOT="$_objc3_selected"
unset _objc3_selected _objc3_previous _objc3_path _objc3_rest _objc3_entry
export OBJC3C_CLANG_FOUNDATION_ROOT="$OBJC3C_SDK_ROOT"
export LLVM_ROOT="$OBJC3C_SDK_ROOT"
export OBJC3C_LLVM_ROOT="$OBJC3C_SDK_ROOT"
export LLVM_DIR="$OBJC3C_SDK_ROOT/lib/cmake/llvm"
export Clang_DIR="$OBJC3C_SDK_ROOT/lib/cmake/clang"
export LD_LIBRARY_PATH="$OBJC3C_SDK_ROOT/lib"
'''.replace('SDK_PATH', shlex.quote(str(root / 'current')))
    # Prepare the stable reader before publishing a new generation. Replacing
    # this reader is safe during migration: it still selects the old current.
    # Subsequent updates only need the atomic pointer replacement below.
    if script.is_symlink() or (script.exists() and not script.is_file()):
        raise ValueError('activation script must be an owned regular file')
    nonce = f'{os.getpid()}-{uuid.uuid4().hex}'
    pending = root / f'.activate-{nonce}'
    temporary = root / f'.current-{nonce}'
    try:
        if not script.exists() or script.read_text() != text:
            pending.write_text(text)
            pending.chmod(0o644)
            os.replace(pending, script)
        temporary.symlink_to(prefix.relative_to(root))
    except BaseException:
        pending.unlink(missing_ok=True)
        temporary.unlink(missing_ok=True)
        raise
    # The pointer replacement commits activation. The caller reports any
    # later staging-directory cleanup error separately.
    try:
        os.replace(temporary, active)
    except BaseException:
        temporary.unlink(missing_ok=True)
        raise


def install(archive: Path, expected_sha256: str, root: Path) -> dict:
    started = time.monotonic()
    costs = {}
    if platform.system() != 'Linux' or platform.machine().lower() not in ('x86_64', 'amd64', 'aarch64', 'arm64'):
        raise ValueError('this SDK requires native Linux x86-64 or AArch64')
    archive = archive.resolve(strict=True)
    root = root.expanduser().resolve()
    if len(expected_sha256) != 64 or file_hash(archive) != expected_sha256.lower():
        raise ValueError('archive SHA-256 does not match the release receipt')
    costs['archive_admission'] = time.monotonic()-started
    archive_bytes = archive.stat().st_size
    decoder_name = shutil.which('zstd')
    if not decoder_name: raise ValueError('SDK installation requires zstd')
    decoder_path = Path(decoder_name).resolve(strict=True)
    decoder_sha256 = file_hash(decoder_path)
    measurement_host = {'system': '-'.join((platform.system(), platform.release(), platform.version())), 'machine': platform.machine(),
        'python_version': platform.python_version(),
        'decoder': {'path': str(decoder_path), 'sha256': decoder_sha256},
        'affinity': sorted(os.sched_getaffinity(0)),
        'cpu_quota': Path('/sys/fs/cgroup/cpu.max').read_text().strip()
            if Path('/sys/fs/cgroup/cpu.max').is_file() else None,
        'memory_limit': Path('/sys/fs/cgroup/memory.max').read_text().strip()
            if Path('/sys/fs/cgroup/memory.max').is_file() else None,
        'cpu_model': next((line.split(':', 1)[1].strip() for line in Path('/proc/cpuinfo').read_text().splitlines()
                           if line.startswith('model name')), None)}
    create_install_directory(root, parents=True)
    cleanup_warnings = []
    with (root / '.install.lock').open('a+b') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        temporary = tempfile.TemporaryDirectory(prefix='.install-', dir=root)
        committed = False
        try:
            phase_started = time.monotonic()
            existing = existing_generation(root, archive, decoder_path)
            if existing is None:
                destination = Path(temporary.name)
                with subprocess.Popen([str(decoder_path), '-dc', str(archive)], stdout=subprocess.PIPE) as decoder:
                    try:
                        with tarfile.open(fileobj=decoder.stdout, mode='r|') as tar:
                            tar.extractall(destination, filter='data')
                        # Drain valid trailing padding after tar's end marker.
                        with open(os.devnull, 'wb') as sink:
                            shutil.copyfileobj(decoder.stdout, sink)
                    finally:
                        decoder.stdout.close()
                    if decoder.wait() != 0:
                        raise ValueError('SDK archive decompression failed')
            costs['extraction'] = time.monotonic()-phase_started
            phase_started = time.monotonic()
            if file_hash(decoder_path) != decoder_sha256:
                raise ValueError('SDK archive decoder changed during installation')
            if existing is not None:
                prefix, seal, verifier_bytes = existing
                data = verify(prefix, verifier_bytes=verifier_bytes)
                if data['seal_sha256'] != seal:
                    raise ValueError('existing SDK prefix differs from the release seal')
                admit_native_sdk(prefix, data)
                reused = True
            else:
                if {path.name for path in destination.iterdir()} != {'objc3-sdk'}:
                    raise ValueError('SDK archive must contain exactly one objc3-sdk directory')
                candidate = destination / 'objc3-sdk'
                # The data filter discards directory modes. Restore the
                # sealed distribution modes under any installation umask.
                for directory in (candidate, *candidate.rglob('*')):
                    if not directory.is_symlink() and directory.is_dir():
                        directory.chmod(0o755)
                data = verify(candidate)
                admit_native_sdk(candidate, data)
                prefix = root / 'sdks' / data['seal_sha256']
                create_install_directory(prefix.parent)
                reused = prefix.exists()
                if reused:
                    if verify(prefix, verifier=candidate / 'share/objc3c/verify-sdk.py')['seal_sha256'] != data['seal_sha256']:
                        raise ValueError('existing SDK prefix differs from the release seal')
                else:
                    os.replace(candidate, prefix)
            costs['seal_verification_and_publication'] = time.monotonic()-phase_started
            phase_started = time.monotonic()
            activate(root, prefix)
            costs['activation'] = time.monotonic()-phase_started
            committed = True
        finally:
            phase_started = time.monotonic()
            try:
                temporary.cleanup()
            except OSError as error:
                if not committed:
                    raise
                cleanup_warnings.append({'path': temporary.name, 'error': str(error)})
            costs['cleanup'] = time.monotonic()-phase_started
    return {'status': 'PASS', 'prefix': str(prefix), 'activation': str(root / 'activate.sh'),
            'seal_sha256': data['seal_sha256'], 'activated': True,
            'cleanup_warnings': cleanup_warnings,
            'cost': {'schema': 'objc3c.sdk-install-cost.v1', 'archive_sha256': expected_sha256.lower(),
                'source_build_id': data['source_build_id'], 'seal_sha256': data['seal_sha256'],
                'host': measurement_host,
                'archive_bytes': archive_bytes, 'prefix_reused': reused,
                'phases_seconds': costs, 'elapsed_seconds': time.monotonic()-started}}


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive', type=Path, required=True)
    parser.add_argument('--sha256', required=True)
    parser.add_argument('--root', type=Path, default=Path.home() / '.local/objc3c')
    args = parser.parse_args(argv)
    try:
        print(json.dumps(install(args.archive, args.sha256, args.root), indent=2))
        return 0
    except (OSError, ValueError, KeyError, subprocess.SubprocessError, tarfile.TarError) as error:
        parser.exit(2, f'objc3c install: {error}\n')


if __name__ == '__main__':
    raise SystemExit(main())
