#!/usr/bin/env python3
"""Lane-D readiness runner for M265-D003."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
COMMANDS = [
    [sys.executable, "scripts/check_m265_c003_typed_keypath_artifact_emission_and_erased_generic_preservation_core_feature_expansion.py"],
    [sys.executable, "scripts/check_m265_d002_live_optional_send_and_key_path_runtime_support_core_feature_implementation.py"],
    [sys.executable, "scripts/check_m265_d003_cross_module_type_surface_preservation_hardening_edge_case_and_compatibility_completion.py"],
]

for command in COMMANDS:
    result = subprocess.run(command, cwd=ROOT, check=False)
    if result.returncode != 0:
        raise SystemExit(result.returncode)

print("[ok] M265-D003 lane-D readiness chain completed")
