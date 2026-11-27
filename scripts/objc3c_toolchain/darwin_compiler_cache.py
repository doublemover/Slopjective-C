"""Admit exact compiler products independently of Darwin runtime sources."""
from __future__ import annotations

import json
from pathlib import Path
import shutil
import tempfile

from .darwin_bootstrap import verify_macho
from .package import _digest, inventory

SCHEMA = 'objc3c.darwin-compiler-cache.v1'
LICENSES = tuple(f'{project}/LICENSE.TXT' for project in ('llvm', 'clang', 'compiler-rt'))
FILES = {'bin/clang', *LICENSES}


def restore(root: Path | None, inputs: dict) -> Path | None:
    if root is None:
        return None
    candidate = root / _digest(inputs)
    if not candidate.exists() and not candidate.is_symlink():
        return None
    if candidate.is_symlink() or (candidate / 'compiler.json').is_symlink():
        raise ValueError('Darwin compiler cache cannot contain symlinks')
    receipt = json.loads((candidate / 'compiler.json').read_bytes())
    files = {name: row for name, row in inventory(candidate).items()
             if name != 'compiler.json' and row['kind'] != 'directory'}
    if (set(files) != FILES or any(row['kind'] != 'file' for row in files.values()) or
            receipt != {'schema': SCHEMA, 'inputs': inputs, 'files': files} or
            not files['bin/clang']['mode'] & 0o111):
        raise ValueError('Darwin compiler cache differs from its configuration or product seal')
    verify_macho(candidate / 'bin/clang', 'executable')
    return candidate


def save(root: Path, inputs: dict, clang: Path, source: Path) -> Path:
    root.mkdir(parents=True, exist_ok=True)
    if existing := restore(root, inputs):
        return existing
    destination = root / _digest(inputs)
    with tempfile.TemporaryDirectory(prefix='.compiler-', dir=root) as directory:
        candidate = Path(directory) / 'products'
        for name, path in [('bin/clang', clang), *((name, source / name) for name in LICENSES)]:
            target = candidate / name
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(path, target).chmod(0o755 if name == 'bin/clang' else 0o644)
        verify_macho(candidate / 'bin/clang', 'executable')
        receipt = {'schema': SCHEMA, 'inputs': inputs, 'files': {
            name: row for name, row in inventory(candidate).items() if row['kind'] != 'directory'}}
        (candidate / 'compiler.json').write_text(json.dumps(receipt, sort_keys=True) + '\n')
        try:
            candidate.rename(destination)
        except FileExistsError:
            if not restore(root, inputs):
                raise
    return destination
