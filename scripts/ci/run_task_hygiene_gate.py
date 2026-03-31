#!/usr/bin/env python3
from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BYTECODE_STASH_ROOT = ROOT / "tmp" / "reports" / "task-hygiene-bytecode"
SEQUENCE = [
    ('task-hygiene', [sys.executable, 'scripts/ci/check_task_hygiene.py']),
    ('governance-budget-enforcement', [sys.executable, 'scripts/check_governance_sustainability_budget_enforcement.py']),
    ('dependency-boundaries', [sys.executable, 'scripts/check_objc3c_dependency_boundaries.py', '--strict']),
    ('repo-superclean-surface', [sys.executable, 'scripts/objc3c_public_workflow_runner.py', 'check-repo-superclean-surface']),
    ('site-index-drift', [sys.executable, 'scripts/objc3c_public_workflow_runner.py', 'check-site']),
    ('native-docs-drift', [sys.executable, 'scripts/objc3c_public_workflow_runner.py', 'check-native-docs']),
    ('public-command-surface-drift', [sys.executable, 'scripts/objc3c_public_workflow_runner.py', 'check-public-command-surface']),
    ('documentation-surface', [sys.executable, 'scripts/objc3c_public_workflow_runner.py', 'check-documentation-surface']),
]


def iter_live_pycache_dirs() -> list[Path]:
    return [path for path in ROOT.rglob("__pycache__") if "tmp" not in path.parts]


def iter_live_pyc_files() -> list[Path]:
    return [path for path in ROOT.rglob("*.pyc") if "tmp" not in path.parts]


def unique_stash_path(relative_path: Path) -> Path:
    destination = BYTECODE_STASH_ROOT / relative_path
    if not destination.exists():
        return destination
    suffix = 1
    while True:
        candidate = destination.with_name(f"{destination.name}.{suffix}")
        if not candidate.exists():
            return candidate
        suffix += 1


def relocate_live_bytecode() -> None:
    moved_paths: list[str] = []
    for path in iter_live_pycache_dirs():
        relative_path = path.relative_to(ROOT)
        destination = unique_stash_path(relative_path)
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.move(str(path), str(destination))
        moved_paths.append(relative_path.as_posix())
    for path in iter_live_pyc_files():
        if not path.exists():
            continue
        relative_path = path.relative_to(ROOT)
        destination = unique_stash_path(relative_path)
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.move(str(path), str(destination))
        moved_paths.append(relative_path.as_posix())
    if moved_paths:
        print(
            f"[check:task-hygiene] relocated live Python bytecode to tmp: {', '.join(moved_paths)}",
            flush=True,
        )


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description='Run the compact task-hygiene gate.')
    parser.add_argument('--dry-run', action='store_true')
    args = parser.parse_args(argv)
    if args.dry_run:
        for name, cmd in SEQUENCE:
            print(f"{name} => {' '.join(cmd)}")
        return 0
    relocate_live_bytecode()
    for name, cmd in SEQUENCE:
        print(f'[check:task-hygiene] {name}', flush=True)
        env = os.environ.copy()
        env['PYTHONDONTWRITEBYTECODE'] = '1'
        rc = subprocess.run(cmd, cwd=ROOT, check=False, env=env).returncode
        if rc != 0:
            return rc
    print(f'[check:task-hygiene] sequence complete ({len(SEQUENCE)} command(s))', flush=True)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
