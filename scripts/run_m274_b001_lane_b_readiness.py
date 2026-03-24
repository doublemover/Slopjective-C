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
        [sys.executable, str(ROOT / "scripts" / "build_objc3c_native_docs.py")],
        [
            sys.executable,
            str(ROOT / "scripts" / "check_m274_b001_part11_interop_semantic_model_contract_and_architecture_freeze.py"),
            "--skip-dynamic-probes",
        ],
        [
            sys.executable,
            "-m",
            "pytest",
            str(
                ROOT
                / "tests"
                / "tooling"
                / "test_check_m274_b001_part11_interop_semantic_model_contract_and_architecture_freeze.py"
            ),
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
