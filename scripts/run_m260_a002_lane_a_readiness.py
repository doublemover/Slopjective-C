#!/usr/bin/env python3
"""Run the focused M260-A002 lane-A readiness stack."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CHECKER = ROOT / "scripts" / "check_m260_a002_ownership_attribute_surface_for_runtime_backed_objects_and_members_core_feature_implementation.py"
TEST = ROOT / "tests" / "tooling" / "test_check_m260_a002_ownership_attribute_surface_for_runtime_backed_objects_and_members_core_feature_implementation.py"


def run(cmd: list[str]) -> None:
    subprocess.run(cmd, cwd=ROOT, check=True)


def main() -> int:
    run([sys.executable, "-m", "pytest", str(TEST), "-q"])
    run([sys.executable, str(CHECKER)])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
