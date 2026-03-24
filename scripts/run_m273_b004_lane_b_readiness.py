#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
STEPS = [
    [sys.executable, str(ROOT / "scripts" / "check_m273_b004_property_behavior_legality_and_interaction_completion_edge_case_and_compatibility_completion.py")],
    [sys.executable, "-m", "pytest", str(ROOT / "tests" / "tooling" / "test_check_m273_b004_property_behavior_legality_and_interaction_completion_edge_case_and_compatibility_completion.py"), "-q"],
]

for step in STEPS:
    completed = subprocess.run(step, cwd=ROOT.parent, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)
