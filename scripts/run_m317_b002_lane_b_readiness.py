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
        [sys.executable, str(ROOT / "tmp" / "github-publish" / "cleanup_acceleration_program" / "apply_m317_b002_existing_amendments.py")],
        [
            sys.executable,
            str(ROOT / "scripts" / "check_m317_b002_existing_milestone_and_issue_amendments_implementation.py"),
            "--skip-github-probes",
        ],
        [
            sys.executable,
            "-m",
            "pytest",
            str(
                ROOT
                / "tests"
                / "tooling"
                / "test_check_m317_b002_existing_milestone_and_issue_amendments_implementation.py"
            ),
            "-q",
        ],
        [
            sys.executable,
            str(ROOT / "scripts" / "check_m317_b002_existing_milestone_and_issue_amendments_implementation.py"),
        ],
    ]
    for command in commands:
        rc = run(command)
        if rc != 0:
            return rc
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
