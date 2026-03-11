#!/usr/bin/env python3
"""Run the focused M276-A001 lane-A readiness chain."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CHECKER = ROOT / "scripts" / "check_m276_a001_native_build_command_surface_and_incremental_build_contract.py"
TEST = ROOT / "tests" / "tooling" / "test_check_m276_a001_native_build_command_surface_and_incremental_build_contract.py"


def run(cmd: list[str]) -> None:
    subprocess.run(cmd, cwd=ROOT, check=True)


def main() -> int:
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, str(CHECKER)])
    run([sys.executable, "-m", "pytest", str(TEST), "-q"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
