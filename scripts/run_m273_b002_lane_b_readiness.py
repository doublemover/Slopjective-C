#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
STEPS = [
    [sys.executable, str(ROOT / "scripts" / "check_m273_b002_derive_expansion_implementation_core_feature_implementation.py")],
    [sys.executable, "-m", "pytest", str(ROOT / "tests" / "tooling" / "test_check_m273_b002_derive_expansion_implementation_core_feature_implementation.py"), "-q"],
]

for step in STEPS:
    completed = subprocess.run(step, cwd=ROOT.parent, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)
