#!/usr/bin/env python3
"""Run the focused M261-C002 lane-C readiness chain."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"


def run(cmd: list[str]) -> None:
    subprocess.run(cmd, cwd=ROOT, check=True)


def main() -> int:
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([NPM_EXECUTABLE, "run", "build:objc3c-native"])
    run([
        sys.executable,
        "scripts/check_m261_c001_block_lowering_abi_and_artifact_boundary_contract_and_architecture_freeze.py",
        "--skip-dynamic-probes",
    ])
    run([
        sys.executable,
        "scripts/check_m261_c002_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation.py",
    ])
    run([
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m261_c002_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation.py",
        "-q",
    ])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
