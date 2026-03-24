#!/usr/bin/env python3
"""Lane-C readiness for M272-C001."""

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
    run([sys.executable, "scripts/check_m272_b003_compatibility_diagnostics_for_dynamism_controls_edge_case_and_compatibility_completion.py", "--skip-dynamic-probes"])
    run([sys.executable, "scripts/check_m272_c001_dispatch_control_lowering_contract_and_architecture_freeze.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m272_c001_dispatch_control_lowering_contract_and_architecture_freeze.py", "-q"])
    print("[info] dependency continuity token: M272-B003 + M272-C001 (Part 9 sema legality and compatibility packets now feed one truthful lowering contract before later direct-call/runtime realization work)")
    print("[ok] M272-C001 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
