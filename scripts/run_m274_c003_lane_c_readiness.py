#!/usr/bin/env python3
"""Readiness runner for M274-C003."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

COMMANDS = [
    [sys.executable, str(ROOT / 'scripts' / 'ensure_objc3c_native_build.py'), '--mode', 'fast', '--reason', 'm274-c003-readiness', '--summary-out', 'tmp/reports/m274/M274-C003/ensure_objc3c_native_build_summary.json'],
    [sys.executable, str(ROOT / 'scripts' / 'check_m274_c002_foreign_call_and_lifetime_lowering_core_feature_implementation.py'), '--skip-dynamic-probes'],
    [sys.executable, str(ROOT / 'scripts' / 'check_m274_c003_metadata_and_interface_preservation_across_ffi_boundaries_core_feature_expansion.py')],
]


def main() -> int:
    for command in COMMANDS:
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            return completed.returncode
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
