#!/usr/bin/env python3
"""Run the M259-C002 lane-C readiness chain."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    print(f"[run] {' '.join(command)}")
    completed = subprocess.run(command, cwd=str(ROOT), check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print(
        "[info] dependency continuity token: M258-E002 + M259-C001 + M259-C002 "
        "(lane C turns the frozen replay/inspection boundary into live IR/object replay and metadata section proof before M259-D001 takes runtime/toolchain operations)"
    )
    run([sys.executable, "scripts/check_m259_c001_end_to_end_replay_and_inspection_contract_and_architecture_freeze.py", "--skip-dynamic-probes"])
    run([sys.executable, "scripts/check_m259_c002_object_and_ir_replay_proof_plus_metadata_inspection_evidence_core_feature_implementation.py"])
    run([
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m259_c002_object_and_ir_replay_proof_plus_metadata_inspection_evidence_core_feature_implementation.py",
        "-q",
    ])
    print("[ok] M259-C002 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
