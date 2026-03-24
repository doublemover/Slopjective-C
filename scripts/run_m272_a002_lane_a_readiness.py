#!/usr/bin/env python3
"""Lane-A readiness for M272-A002."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    run([sys.executable, "scripts/check_m272_a001_direct_final_sealed_and_dynamism_control_source_closure_contract_and_architecture_freeze.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m272_a001_direct_final_sealed_and_dynamism_control_source_closure_contract_and_architecture_freeze.py", "-q"])
    run([sys.executable, "scripts/check_m272_a002_frontend_attribute_and_defaulting_surface_completion_core_feature_implementation.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m272_a002_frontend_attribute_and_defaulting_surface_completion_core_feature_implementation.py", "-q"])
    print("[info] dependency continuity token: M272-A001 + M272-A002 (Part 9 now preserves both raw dispatch-intent attribute admission and prefixed/defaulted direct-members frontend completion before later M272 legality and lowering work)")
    print("[ok] M272-A002 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
