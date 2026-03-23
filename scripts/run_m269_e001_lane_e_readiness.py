#!/usr/bin/env python3
"""Lane-E readiness runner for M269-E001."""

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
            "m269-e001-lane-e-readiness",
            "--summary-out",
            "tmp/reports/m269/M269-E001/ensure_objc3c_native_build_summary.json",
        ],
    ),
    ("build-native-docs", [sys.executable, str(ROOT / "scripts" / "build_objc3c_native_docs.py")]),
    ("m269-a002-lane-a", [sys.executable, str(ROOT / "scripts" / "run_m269_a002_lane_a_readiness.py")]),
    ("m269-b003-lane-b", [sys.executable, str(ROOT / "scripts" / "run_m269_b003_lane_b_readiness.py")]),
    ("m269-c003-lane-c", [sys.executable, str(ROOT / "scripts" / "run_m269_c003_lane_c_readiness.py")]),
    ("m269-d003-lane-d", [sys.executable, str(ROOT / "scripts" / "run_m269_d003_lane_d_readiness.py")]),
    ("m269-e001-check", [sys.executable, str(ROOT / "scripts" / "check_m269_e001_task_and_executor_conformance_gate_contract_and_architecture_freeze.py")]),
    (
        "m269-e001-pytest",
        [
            sys.executable,
            "-m",
            "pytest",
            str(ROOT / "tests" / "tooling" / "test_check_m269_e001_task_and_executor_conformance_gate_contract_and_architecture_freeze.py"),
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
    print("[ok] M269-E001 lane-E readiness passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
