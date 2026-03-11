#!/usr/bin/env python3
"""Run the focused M261-C004 lane-C readiness chain."""

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
        "scripts/check_m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion.py",
        "--skip-dynamic-probes",
    ])
    run([
        sys.executable,
        "scripts/check_m261_c003_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation.py",
        "--skip-dynamic-probes",
    ])
    run([
        sys.executable,
        "scripts/check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py",
    ])
    run([
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py",
        "-q",
    ])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
