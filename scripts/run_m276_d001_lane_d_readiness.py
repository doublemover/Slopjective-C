#!/usr/bin/env python3
"""Readiness runner for M276-D001."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DOCS_BUILD = ROOT / "scripts" / "build_objc3c_native_docs.py"
CHECKER = ROOT / "scripts" / "check_m276_d001_readiness_runner_and_checker_migration_to_fast_vs_full_validation_policy.py"
TEST = ROOT / "tests" / "tooling" / "test_check_m276_d001_readiness_runner_and_checker_migration_to_fast_vs_full_validation_policy.py"


def run(cmd: list[str]) -> None:
    subprocess.run(cmd, cwd=ROOT, check=True)


def main() -> int:
    run([sys.executable, str(DOCS_BUILD)])
    run([sys.executable, str(CHECKER)])
    run([sys.executable, "-m", "pytest", str(TEST), "-q"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
