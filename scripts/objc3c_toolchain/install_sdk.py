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
import types


def file_hash(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def verify(prefix: Path) -> dict:
    # Loading a module through importlib writes __pycache__ unless the parent
    # happens to use -B. A verifier must never mutate the sealed installation.
    path = prefix / 'share/objc3c/verify-sdk.py'
    module = types.ModuleType('objc3_sdk_verifier')
    module.__file__ = str(path)
    exec(compile(path.read_bytes(), str(path), 'exec'), module.__dict__)
    return module.verify_sdk(prefix)


def activate(root: Path, prefix: Path) -> None:
    active = root / 'current'
    if active.exists() and not active.is_symlink():
        raise ValueError(f'activation path is not a symbolic link; preserving {active}')
    temporary = root / f'.current-{os.getpid()}'
    try:
        temporary.symlink_to(prefix.relative_to(root))
        os.replace(temporary, active)
    finally:
        temporary.unlink(missing_ok=True)
    script = root / 'activate.sh'
    text = '''# Source this file in a POSIX shell. Bootstrap and product SDKs are separate.
if [ -z "${OBJC3C_BOOTSTRAP_LLVM_ROOT:-}" ] && [ -n "${LLVM_ROOT:-}" ] && [ ! -f "$LLVM_ROOT/objc3-sdk.json" ]; then
  export OBJC3C_BOOTSTRAP_LLVM_ROOT="$LLVM_ROOT"
fi
export OBJC3C_SDK_ROOT=SDK_PATH
export OBJC3C_CLANG_FOUNDATION_ROOT="$OBJC3C_SDK_ROOT"
export LLVM_ROOT="$OBJC3C_SDK_ROOT"
export OBJC3C_LLVM_ROOT="$OBJC3C_SDK_ROOT"
export LLVM_DIR="$OBJC3C_SDK_ROOT/lib/cmake/llvm"
export Clang_DIR="$OBJC3C_SDK_ROOT/lib/cmake/clang"
export PATH="$OBJC3C_SDK_ROOT/bin:$PATH"
export LD_LIBRARY_PATH="$OBJC3C_SDK_ROOT/lib"
'''.replace('SDK_PATH', shlex.quote(str(prefix)))
    pending = root / f'.activate-{os.getpid()}'
    try:
        pending.write_text(text)
        os.replace(pending, script)
    finally:
        pending.unlink(missing_ok=True)


def install(archive: Path, expected_sha256: str, root: Path) -> dict:
    if platform.system() != 'Linux' or platform.machine() != 'x86_64':
        raise ValueError('this SDK requires native Linux x86-64')
    archive = archive.resolve(strict=True)
    root = root.expanduser().resolve()
    if len(expected_sha256) != 64 or file_hash(archive) != expected_sha256.lower():
        raise ValueError('archive SHA-256 does not match the release receipt')
    root.mkdir(parents=True, exist_ok=True)
    with (root / '.install.lock').open('a+b') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        with tempfile.TemporaryDirectory(prefix='.install-', dir=root) as temporary:
            destination = Path(temporary)
            with subprocess.Popen(['zstd', '-dc', str(archive)], stdout=subprocess.PIPE) as decoder:
                try:
                    with tarfile.open(fileobj=decoder.stdout, mode='r|') as tar:
                        tar.extractall(destination, filter='data')
                finally:
                    decoder.stdout.close()
                if decoder.wait() != 0:
                    raise ValueError('SDK archive decompression failed')
            if {path.name for path in destination.iterdir()} != {'objc3-sdk'}:
                raise ValueError('SDK archive must contain exactly one objc3-sdk directory')
            candidate = destination / 'objc3-sdk'
            data = verify(candidate)
            prefix = root / 'sdks' / data['seal_sha256']
            prefix.parent.mkdir(exist_ok=True)
            if prefix.exists():
                if verify(prefix)['seal_sha256'] != data['seal_sha256']:
                    raise ValueError('existing SDK prefix differs from the release seal')
            else:
                os.replace(candidate, prefix)
            activate(root, prefix)
    return {'status': 'PASS', 'prefix': str(prefix), 'activation': str(root / 'activate.sh'),
            'seal_sha256': data['seal_sha256']}


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
