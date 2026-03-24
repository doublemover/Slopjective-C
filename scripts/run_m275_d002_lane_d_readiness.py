#!/usr/bin/env python3
"""Lane-D readiness runner for M275-D002."""

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
    run([sys.executable, "scripts/check_m275_d002_toolchain_operations_and_evidence_publication_core_feature_implementation.py"])
    run([
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m275_d002_toolchain_operations_and_evidence_publication_core_feature_implementation.py",
        "-q",
    ])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
