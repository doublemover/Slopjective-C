"""Content identity of the native frontend implementation and CMake inputs."""
from __future__ import annotations

import hashlib
import json
from pathlib import Path


def facade_identity(repo: Path) -> dict:
    repo = repo.resolve()
    root = repo / 'native/objc3c'
    files = {root / 'CMakeLists.txt', repo / 'scripts/objc3c_toolchain/facade.py',
             root / 'src/runtime/public/objc3_runtime_services.h',
             root / 'src/runtime/public/objc3_task.hpp',
             root / 'src/runtime/public/objc3_keypath.h'}
    library = repo / 'stdlib'
    catalog = library / 'module_inventory.json'
    files.add(catalog)
    inventory = json.loads(catalog.read_text())
    for module in inventory['canonical_modules']:
        header = (library / module['header']).resolve(strict=True)
        if not header.is_relative_to(library):
            raise ValueError('standard-library header escapes its source root')
        files.add(header)
    files.update((root / 'cmake').glob('*.cmake*'))
    files.update(path for path in (root / 'src/clang_foundation').rglob('*')
                 if path.suffix in {'.cpp', '.h'} or path.name.endswith('.h.in')
                 or path.name == 'CMakeLists.txt')
    if not (root / 'src/clang_foundation/compile_session.cpp').is_file():
        raise ValueError('native frontend implementation is missing')
    inputs = {str(path.relative_to(repo).as_posix()): hashlib.sha256(path.read_bytes()).hexdigest()
              for path in sorted(files)}
    digest = hashlib.sha256(json.dumps(inputs, sort_keys=True, separators=(',', ':')).encode()).hexdigest()
    return {'sha256': digest, 'inputs': sorted(inputs)}
