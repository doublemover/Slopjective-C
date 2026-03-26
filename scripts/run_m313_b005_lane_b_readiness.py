#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> int:
    return subprocess.run(command, cwd=ROOT).returncode


def main() -> int:
    commands = [
        [sys.executable, str(ROOT / "scripts" / "check_m313_b005_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion.py")],
        [
            sys.executable,
            "-m",
            "pytest",
            str(ROOT / "tests" / "tooling" / "test_check_m313_b005_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion.py"),
            "-q",
        ],
    ]
    for command in commands:
        rc = run(command)
        if rc != 0:
            return rc
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
