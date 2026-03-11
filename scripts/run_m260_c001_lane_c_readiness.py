#!/usr/bin/env python3
"""Run the focused M260-C001 lane-C readiness stack."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CHECKER = ROOT / "scripts" / "check_m260_c001_ownership_lowering_baseline_contract_and_architecture_freeze.py"
TEST = ROOT / "tests" / "tooling" / "test_check_m260_c001_ownership_lowering_baseline_contract_and_architecture_freeze.py"
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"


def run(cmd: list[str]) -> None:
    subprocess.run(cmd, cwd=ROOT, check=True)


def main() -> int:
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, "scripts/build_site_index.py"])
    run([NPM_EXECUTABLE, "run", "build:objc3c-native"])
    run([sys.executable, "-m", "pytest", str(TEST), "-q"])
    run([sys.executable, str(CHECKER)])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
