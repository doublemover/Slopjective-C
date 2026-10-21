"""Content identity of the native frontend implementation and CMake inputs."""
from __future__ import annotations

import hashlib
import json
from pathlib import Path


def facade_identity(repo: Path) -> dict:
    repo = repo.resolve()
    root = repo / 'native/objc3c'
    files = {root / 'CMakeLists.txt', repo / 'scripts/objc3c_toolchain/facade.py'}
    files.update((root / 'cmake').glob('*.cmake*'))
    files.update(path for path in (root / 'src/clang_foundation').rglob('*')
                 if path.suffix in {'.cpp', '.h'} or path.name == 'CMakeLists.txt')
    if not (root / 'src/clang_foundation/compile_session.cpp').is_file():
        raise ValueError('native frontend implementation is missing')
    inputs = {str(path.relative_to(repo).as_posix()): hashlib.sha256(path.read_bytes()).hexdigest()
              for path in sorted(files)}
    digest = hashlib.sha256(json.dumps(inputs, sort_keys=True, separators=(',', ':')).encode()).hexdigest()
    return {'sha256': digest, 'inputs': sorted(inputs)}
