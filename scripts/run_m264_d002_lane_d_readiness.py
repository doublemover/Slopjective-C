#!/usr/bin/env python3
"""Lane-D readiness runner for M264-D002."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]


def run(command: Sequence[str]) -> None:
    completed = subprocess.run(
        list(command),
        cwd=str(ROOT),
        check=False,
        text=True,
        encoding="utf-8",
        errors="replace",
    )
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M264-D001 + M264-C002 (explicit conformance operations must stay bounded to the already emitted truthful conformance/runtime sidecars)")
    run(["python", "scripts/check_m264_d001_driver_and_profile_selection_report_publication_contract_and_architecture_freeze.py"])
    run(["python", "scripts/run_m264_c002_lane_c_readiness.py"])
    run(["python", "scripts/check_m264_d002_cli_and_toolchain_conformance_claim_operations_core_feature_implementation.py"])
    run([
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m264_d002_cli_and_toolchain_conformance_claim_operations_core_feature_implementation.py",
        "-q",
    ])
    print("[ok] M264-D002 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
