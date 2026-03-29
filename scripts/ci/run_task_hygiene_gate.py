#!/usr/bin/env python3
from __future__ import annotations

import argparse
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SEQUENCE = [
    ('task-hygiene', [sys.executable, 'scripts/ci/check_task_hygiene.py']),
    ('dependency-boundaries', [sys.executable, 'scripts/check_objc3c_dependency_boundaries.py', '--strict']),
    ('site-index-drift', [sys.executable, 'scripts/objc3c_public_workflow_runner.py', 'check-site-index']),
    ('native-docs-drift', [sys.executable, 'scripts/objc3c_public_workflow_runner.py', 'check-native-docs']),
    ('public-command-surface-drift', [sys.executable, 'scripts/objc3c_public_workflow_runner.py', 'check-public-command-surface']),
    ('documentation-surface', [sys.executable, 'scripts/objc3c_public_workflow_runner.py', 'check-documentation-surface']),
]


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description='Run the compact task-hygiene gate.')
    parser.add_argument('--dry-run', action='store_true')
    args = parser.parse_args(argv)
    if args.dry_run:
        for name, cmd in SEQUENCE:
            print(f"{name} => {' '.join(cmd)}")
        return 0
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
