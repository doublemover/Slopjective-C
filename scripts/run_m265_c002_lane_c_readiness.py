#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
COMMANDS = [
    [sys.executable, "scripts/run_m265_c001_lane_c_readiness.py"],
    [sys.executable, "scripts/check_m265_c002_optional_chaining_binding_and_coalescing_lowering_core_feature_implementation.py"],
    [sys.executable, "-m", "pytest", "tests/tooling/test_check_m265_c002_optional_chaining_binding_and_coalescing_lowering_core_feature_implementation.py", "-q"],
]

for command in COMMANDS:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        sys.exit(completed.returncode)

print("[ok] M265-C002 lane-C readiness chain completed")
