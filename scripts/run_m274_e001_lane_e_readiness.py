#!/usr/bin/env python3
"""Lane-E readiness runner for M274-E001."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

COMMANDS: tuple[tuple[str, list[str]], ...] = (
    (
        "ensure-fast-native-build",
        [
            sys.executable,
            str(ROOT / "scripts" / "ensure_objc3c_native_build.py"),
            "--mode",
            "fast",
            "--reason",
            "m274-e001-lane-e-readiness",
            "--summary-out",
            "tmp/reports/m274/M274-E001/ensure_objc3c_native_build_summary.json",
        ],
    ),
    ("build-native-docs", [sys.executable, str(ROOT / "scripts" / "build_objc3c_native_docs.py")]),
    ("m274-a003-lane-a", [sys.executable, str(ROOT / "scripts" / "run_m274_a003_lane_a_readiness.py")]),
    ("m274-b004-lane-b", [sys.executable, str(ROOT / "scripts" / "run_m274_b004_lane_b_readiness.py")]),
    ("m274-c003-lane-c", [sys.executable, str(ROOT / "scripts" / "run_m274_c003_lane_c_readiness.py")]),
    ("m274-d002-lane-d", [sys.executable, str(ROOT / "scripts" / "run_m274_d002_lane_d_readiness.py")]),
    ("m274-e001-check", [sys.executable, str(ROOT / "scripts" / "check_m274_e001_interop_conformance_gate_contract_and_architecture_freeze.py")]),
    (
        "m274-e001-pytest",
        [
            sys.executable,
            "-m",
            "pytest",
            str(ROOT / "tests" / "tooling" / "test_check_m274_e001_interop_conformance_gate_contract_and_architecture_freeze.py"),
            "-q",
        ],
    ),
)


def main() -> int:
    for label, command in COMMANDS:
        completed = subprocess.run(command, cwd=ROOT, text=True)
        if completed.returncode != 0:
            print(f"[fail] {label}", file=sys.stderr)
            return completed.returncode
    print("[ok] M274-E001 lane-E readiness passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
