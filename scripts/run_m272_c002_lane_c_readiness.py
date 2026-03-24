#!/usr/bin/env python3
"""Lane-C readiness for M272-C002."""

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
    run([sys.executable, "scripts/check_m272_c001_dispatch_control_lowering_contract_and_architecture_freeze.py", "--skip-dynamic-probes"])
    run([sys.executable, "scripts/check_m272_c002_direct_sealed_and_final_dispatch_lowering_core_feature_implementation.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m272_c002_direct_sealed_and_final_dispatch_lowering_core_feature_implementation.py", "-q"])
    print("[info] dependency continuity token: M272-B003 + M272-C001 + M272-C002 (Part 9 compatibility packets now drive live concrete direct-call lowering while metadata preserves final/sealed/direct intent)")
    print("[ok] M272-C002 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
