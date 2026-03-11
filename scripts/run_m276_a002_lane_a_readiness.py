#!/usr/bin/env python3
"""Run the focused M276-A002 lane-A readiness chain."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CHECKER = ROOT / "scripts" / "check_m276_a002_build_graph_and_toolchain_parity_for_incremental_native_builds.py"
TEST = ROOT / "tests" / "tooling" / "test_check_m276_a002_build_graph_and_toolchain_parity_for_incremental_native_builds.py"


def run(cmd: list[str]) -> None:
    subprocess.run(cmd, cwd=ROOT, check=True)


def main() -> int:
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, str(CHECKER)])
    run([sys.executable, "-m", "pytest", str(TEST), "-q"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
