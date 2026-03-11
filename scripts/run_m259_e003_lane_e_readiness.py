#!/usr/bin/env python3
"""Run the focused M259-E003 lane-E readiness stack."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CHECKER = ROOT / "scripts" / "check_m259_e003_runnable_object_model_closeout_signoff.py"
TEST = ROOT / "tests" / "tooling" / "test_check_m259_e003_runnable_object_model_closeout_signoff.py"


def run(cmd: list[str]) -> None:
    subprocess.run(cmd, cwd=ROOT, check=True)


def main() -> int:
    run([sys.executable, "-m", "pytest", str(TEST), "-q"])
    run([sys.executable, str(CHECKER)])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
