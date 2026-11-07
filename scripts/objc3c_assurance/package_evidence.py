"""Retain the SDK owner's nested diagnostics inside the uploaded native suite."""
from __future__ import annotations

import json
from pathlib import Path
import shutil

from .evidence import digest, native_files


def snapshot(repo: Path) -> dict[str, str]:
    root = repo / 'out/toolchains/linux-sdk-builds'
    return {str(path): digest(path) for path in root.glob('*/build-report.json') if path.is_file()}


def preserve(repo: Path, destination: Path, before: dict[str, str]) -> list[str]:
    """Only the active SDK attempt's reports/logs; never SDK/build products."""
    repo = repo.resolve(strict=True)
    destination.mkdir(parents=True, exist_ok=True)
    selected = []

    def copy(root, target):
        for path in native_files(root):
            relative = path.relative_to(root)
            if 'prefix' in relative.parts:
                continue
            if path.is_symlink() or not path.resolve().is_relative_to(root.resolve()):
                raise ValueError('SDK diagnostics escaped their owned evidence tree')
            output = target / relative
            output.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(path, output)

    for name, signature in snapshot(repo).items():
        if before.get(name) == signature:
            continue
        path = Path(name)
        candidate = path.parent
        selected.append(candidate.name)
        copy(candidate, destination / 'candidates' / candidate.name)
        result = json.loads(path.read_text())
        for row in result.get('commands', []):
            if row.get('name') != 'native-validation':
                continue
            argv = row['command']
            run_id = argv[argv.index('--run-id') + 1]
            if not run_id or Path(run_id).name != run_id or run_id in ('.', '..'):
                raise ValueError('SDK native validation has an unowned run identity')
            root = repo / 'out/native-validation' / run_id
            if root.is_dir():
                copy(root, destination / 'native-validation' / run_id)
    return selected
