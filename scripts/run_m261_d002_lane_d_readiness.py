#!/usr/bin/env python3
"""Run the focused M261-D002 lane-D readiness chain."""

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
        "scripts/check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py",
        "--skip-dynamic-probes",
    ])
    run([
        sys.executable,
        "scripts/check_m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze.py",
        "--skip-dynamic-probes",
    ])
    run([
        sys.executable,
        "scripts/check_m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation.py",
    ])
    run([
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation.py",
        "-q",
    ])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
