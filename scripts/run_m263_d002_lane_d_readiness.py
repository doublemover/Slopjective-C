#!/usr/bin/env python3
"""Lane-D readiness runner for M263-D002."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
COMMANDS = (
    ["npm.cmd", "run", "build:objc3c-native"],
    [
        sys.executable,
        "scripts/check_m263_c003_archive_and_static_link_bootstrap_replay_corpus_conformance_corpus_expansion.py",
    ],
    [
        sys.executable,
        "scripts/check_m263_d001_runtime_bootstrap_table_consumption_contract_and_architecture_freeze.py",
    ],
    [
        sys.executable,
        "scripts/check_m263_d002_live_registration_replay_and_discovery_implementation.py",
    ],
    [
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m263_d002_live_registration_replay_and_discovery_implementation.py",
        "-q",
    ],
)


def run_command(command: list[str]) -> int:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    return completed.returncode


def main() -> int:
    for command in COMMANDS:
        print("[run]", " ".join(command))
        code = run_command(command)
        if code != 0:
            return code
    print("[ok] M263-D002 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
