#!/usr/bin/env python3
"""Lane-E readiness runner for M275-E001."""

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
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, "scripts/check_m275_e001_integrated_advanced_feature_conformance_gate_contract_and_architecture_freeze.py"])
    run([
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m275_e001_integrated_advanced_feature_conformance_gate_contract_and_architecture_freeze.py",
        "-q",
    ])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
