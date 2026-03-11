#!/usr/bin/env python3
"""Readiness runner for M276-E001."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DOCS_BUILD = ROOT / "scripts" / "build_objc3c_native_docs.py"
CHECKER = ROOT / "scripts" / "check_m276_e001_package_scripts_operator_workflow_and_developer_runbook_migration_for_incremental_native_builds.py"
TEST = ROOT / "tests" / "tooling" / "test_check_m276_e001_package_scripts_operator_workflow_and_developer_runbook_migration_for_incremental_native_builds.py"


def run(cmd: list[str]) -> None:
    subprocess.run(cmd, cwd=ROOT, check=True)


def main() -> int:
    run([sys.executable, str(DOCS_BUILD)])
    run([sys.executable, str(CHECKER)])
    run([sys.executable, "-m", "pytest", str(TEST), "-q"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
