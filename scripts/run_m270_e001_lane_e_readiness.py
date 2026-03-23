#!/usr/bin/env python3
"""Lane-E readiness runner for M270-E001."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

COMMANDS: tuple[tuple[str, list[str]], ...] = (
    (
        'ensure-fast-native-build',
        [
            sys.executable,
            str(ROOT / 'scripts' / 'ensure_objc3c_native_build.py'),
            '--mode',
            'fast',
            '--reason',
            'm270-e001-lane-e-readiness',
            '--summary-out',
            'tmp/reports/m270/M270-E001/ensure_objc3c_native_build_summary.json',
        ],
    ),
    ('build-native-docs', [sys.executable, str(ROOT / 'scripts' / 'build_objc3c_native_docs.py')]),
    ('m270-a002-lane-a', [sys.executable, str(ROOT / 'scripts' / 'run_m270_a002_lane_a_readiness.py')]),
    ('m270-b003-lane-b', [sys.executable, str(ROOT / 'scripts' / 'run_m270_b003_lane_b_readiness.py')]),
    ('m270-c003-lane-c', [sys.executable, str(ROOT / 'scripts' / 'run_m270_c003_lane_c_readiness.py')]),
    ('m270-d003-lane-d', [sys.executable, str(ROOT / 'scripts' / 'run_m270_d003_lane_d_readiness.py')]),
    ('m270-e001-check', [sys.executable, str(ROOT / 'scripts' / 'check_m270_e001_strict_concurrency_conformance_gate_contract_and_architecture_freeze.py')]),
    (
        'm270-e001-pytest',
        [
            sys.executable,
            '-m',
            'pytest',
            str(ROOT / 'tests' / 'tooling' / 'test_check_m270_e001_strict_concurrency_conformance_gate_contract_and_architecture_freeze.py'),
            '-q',
        ],
    ),
)


def main() -> int:
    for label, command in COMMANDS:
        completed = subprocess.run(command, cwd=ROOT, text=True)
        if completed.returncode != 0:
            print(f'[fail] {label}', file=sys.stderr)
            return completed.returncode
    print('[ok] M270-E001 lane-E readiness passed')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
