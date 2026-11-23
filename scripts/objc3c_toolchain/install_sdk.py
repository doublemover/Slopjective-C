"""Install a verified Linux SDK into a content-addressed prefix and activate it."""
from __future__ import annotations

import argparse
import fcntl
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shlex
import shutil
import subprocess
import sys
import tarfile
import tempfile
import time
import types
import uuid


# Keep the downloaded installer self-contained so it can admit the archive
# without loading modules from an existing or newly extracted SDK.
MACHINES = {'x86_64': ('x86_64-unknown-linux-gnu', 62),
            'amd64': ('x86_64-unknown-linux-gnu', 62),
            'aarch64': ('aarch64-unknown-linux-gnu', 183),
            'arm64': ('aarch64-unknown-linux-gnu', 183)}

LIBSTDCXX_PROBE = '''
import ctypes
import sys

try:
    library = ctypes.CDLL('libstdc++.so.6')
    libc = ctypes.CDLL(None)
    libc.dlvsym.argtypes = (ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p)
    libc.dlvsym.restype = ctypes.c_void_p
    libc.dlerror.restype = ctypes.c_char_p
    symbol = sys.argv[1].encode('ascii')
    libc.dlerror()
    libc.dlvsym(library._handle, symbol, symbol)
    # The linker emits each version node as an absolute symbol at address zero.
    error = libc.dlerror()
    if error is not None:
        raise OSError(error.decode(errors='replace'))
except (AttributeError, OSError) as error:
    sys.exit(str(error))
'''


def file_hash(path: Path) -> str:
    # The standalone installer also reads ABI metadata on distributions with
    # Python older than the Python 3.11 required to verify an admitted SDK.
    digest = hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b''):
            digest.update(block)
    return digest.hexdigest()


def admit_target(data: dict) -> int:
    target, machine = MACHINES[platform.machine().lower()]
    if data.get('target') != target:
        raise ValueError(f'SDK target {data.get("target")!r} differs from the native Linux target '
                         f'{target}. Download the SDK for this architecture.')
    return machine


def admit_host_abi(data: dict) -> dict:
    abi = data.get('linux_abi')
    if not isinstance(abi, dict):
        raise ValueError('SDK manifest is missing Linux ABI requirements')
    glibc = abi.get('glibc_minimum')
    libstdcxx = abi.get('libstdcxx_symbol_minimum')
    if (not isinstance(glibc, str) or not re.fullmatch(r'[0-9]{1,3}\.[0-9]{1,3}', glibc) or
            not isinstance(libstdcxx, str) or not re.fullmatch(r'GLIBCXX_[0-9]{1,3}(?:\.[0-9]{1,3}){1,2}', libstdcxx)):
        raise ValueError('SDK manifest contains invalid Linux ABI requirements')
    try:
        libc = os.confstr('CS_GNU_LIBC_VERSION') or ''
    except (OSError, ValueError):
        libc = ''
    host_glibc = re.fullmatch(r'glibc ([0-9]+\.[0-9]+)', libc)
    if host_glibc is None or tuple(map(int, host_glibc[1].split('.'))) < tuple(map(int, glibc.split('.'))):
        detected = libc or 'GNU libc unavailable'
        raise ValueError(f'SDK requires glibc >= {glibc} (detected {detected}). '
                         'Use an SDK-supported Linux distribution or container.')
    # Resolve the system libstdc++ in a fresh process. A previously activated
    # SDK, loader override, or a library already loaded into Python cannot
    # supply the host ABI for the new installation.
    env = {key: value for key, value in os.environ.items() if not key.startswith('LD_')}
    try:
        probe = subprocess.run([sys.executable, '-I', '-S', '-c', LIBSTDCXX_PROBE, libstdcxx],
                               env=env, capture_output=True, text=True, timeout=10)
        if probe.returncode:
            raise ValueError(probe.stderr.strip() or f'host loader probe exited {probe.returncode}')
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        raise ValueError(f'SDK requires system libstdc++.so.6 with {libstdcxx} ({error}). '
                         'Install or upgrade the distribution libstdc++ package, or use an '
                         'SDK-supported Linux distribution or container.') from None
    return {'glibc': host_glibc[1], 'glibc_minimum': glibc, 'libstdcxx_symbol_verified': libstdcxx}


def admit_native_sdk(prefix: Path, data: dict) -> None:
    machine = admit_target(data)
    for name, row in data['files'].items():
        if row['kind'] != 'file':
            continue
        with (prefix / name).open('rb') as stream:
            header = stream.read(64)
        if header[:4] == b'\x7fELF' and (len(header) != 64 or
                header[:6] != b'\x7fELF\x02\x01' or
                int.from_bytes(header[18:20], 'little') != machine):
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


def read_archive_admission(archive: Path, decoder_path: Path) -> tuple[dict, bytes] | None:
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
    data = json.loads(admission[0])
    seal = data['seal_sha256']
    if not isinstance(seal, str) or not re.fullmatch('[0-9a-f]{64}', seal):
        raise ValueError('SDK archive contains an invalid generation seal')
    return data, admission[1]


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
    if platform.system() != 'Linux' or platform.machine().lower() not in MACHINES:
        raise ValueError('this SDK requires native Linux x86-64 or AArch64')
    archive = archive.resolve(strict=True)
    root = root.expanduser().resolve()
    if len(expected_sha256) != 64 or file_hash(archive) != expected_sha256.lower():
        raise ValueError('archive SHA-256 does not match the release receipt')
    archive_bytes = archive.stat().st_size
    decoder_name = shutil.which('zstd')
    if not decoder_name: raise ValueError('SDK installation requires zstd')
    decoder_path = Path(decoder_name).resolve(strict=True)
    decoder_sha256 = file_hash(decoder_path)
    admission = read_archive_admission(archive, decoder_path)
    host_abi = None
    if admission is not None:
        admit_target(admission[0])
        host_abi = admit_host_abi(admission[0])
    if sys.version_info < (3, 11) or not hasattr(tarfile, 'data_filter'):
        raise ValueError('SDK installation requires distribution Python 3.11 or newer with tarfile data filtering')
    costs['archive_admission'] = time.monotonic()-started
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
            existing = None
            generations = root / 'sdks'
            if admission is not None and generations.is_dir() and not generations.is_symlink():
                prefix = generations / admission[0]['seal_sha256']
                if prefix.is_dir() and not prefix.is_symlink():
                    existing = prefix
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
                candidate = existing
                data = verify(candidate, verifier_bytes=admission[1])
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
            if admission is not None and data['seal_sha256'] != admission[0]['seal_sha256']:
                raise ValueError('verified SDK prefix differs from the archive admission seal')
            admit_native_sdk(candidate, data)
            if host_abi is None:
                host_abi = admit_host_abi(data)
            if existing is not None:
                reused = True
            else:
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
                'host': {**measurement_host, 'linux_abi': host_abi},
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
