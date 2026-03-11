#!/usr/bin/env python3
"""Run the focused M260-B003 lane-B readiness stack."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CHECKER = ROOT / "scripts" / "check_m260_b003_autoreleasepool_and_destruction_order_semantics_for_object_instances_core_feature_expansion.py"
TEST = ROOT / "tests" / "tooling" / "test_check_m260_b003_autoreleasepool_and_destruction_order_semantics_for_object_instances_core_feature_expansion.py"


def run(cmd: list[str]) -> None:
    subprocess.run(cmd, cwd=ROOT, check=True)


def main() -> int:
    run([sys.executable, "-m", "pytest", str(TEST), "-q"])
    run([sys.executable, str(CHECKER)])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
