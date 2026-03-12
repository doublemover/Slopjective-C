#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
COMMANDS = [
    [sys.executable, "scripts/run_m265_a002_lane_a_readiness.py"],
    [sys.executable, "scripts/check_m265_b001_optional_generic_erasure_and_key_path_semantic_model_contract_and_architecture_freeze.py"],
    [sys.executable, "scripts/run_m265_b002_lane_b_readiness.py"],
    [sys.executable, "scripts/check_m265_b003_generic_erasure_and_key_path_legality_completion_edge_case_and_compatibility_completion.py"],
    [sys.executable, "scripts/check_m265_c001_optional_and_key_path_lowering_contract_and_architecture_freeze.py"],
    [sys.executable, "-m", "pytest", "tests/tooling/test_check_m265_c001_optional_and_key_path_lowering_contract_and_architecture_freeze.py", "-q"],
]

for command in COMMANDS:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        sys.exit(completed.returncode)

print("[ok] M265-C001 lane-C readiness chain completed")
