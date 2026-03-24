#!/usr/bin/env python3
"""Lane-A readiness for M272-A001."""

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
    print("[info] dependency continuity token: M271-E002 + M272-A001 (Part 9 now freezes one explicit parser/frontend dispatch-intent source boundary before later M272 legality, lowering, and runtime work)")
    print("[ok] M272-A001 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
