#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
COMMANDS = [
    [sys.executable, "scripts/check_m265_a002_frontend_support_for_optional_sends_binds_coalescing_and_typed_key_paths_core_feature_implementation.py"],
    [sys.executable, "scripts/check_m265_c001_optional_and_key_path_lowering_contract_and_architecture_freeze.py"],
    [sys.executable, "scripts/check_m265_c003_typed_keypath_artifact_emission_and_erased_generic_preservation_core_feature_expansion.py"],
    [sys.executable, "-m", "pytest", "tests/tooling/test_check_m265_c003_typed_keypath_artifact_emission_and_erased_generic_preservation_core_feature_expansion.py", "-q"],
]

for command in COMMANDS:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        sys.exit(completed.returncode)

print("[ok] M265-C003 lane-C readiness chain completed")
