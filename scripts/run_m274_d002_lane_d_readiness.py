#!/usr/bin/env python3
"""Run M274-D002 lane-D readiness without npm nesting."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(args: list[str]) -> None:
    completed = subprocess.run(args, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M274-C002 + M274-C003 + M274-D001 + M274-D002 (Part 11 now emits deterministic header, modulemap, and bridge sidecars locally, preserves those paths through runtime-import surfaces and cross-module link plans, and proves the same boundary through the private Part 11 runtime snapshot)")
    run([sys.executable, str(ROOT / "scripts" / "build_objc3c_native_docs.py")])
    run([sys.executable, str(ROOT / "scripts" / "check_m274_d002_header_module_and_bridge_generation_implementation_core_feature_implementation.py")])
    run([sys.executable, "-m", "pytest", str(ROOT / "tests" / "tooling" / "test_check_m274_d002_header_module_and_bridge_generation_implementation_core_feature_implementation.py"), "-q"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
