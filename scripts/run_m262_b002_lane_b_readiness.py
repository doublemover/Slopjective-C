#!/usr/bin/env python3
"""Run the focused M262-B002 lane-B readiness chain."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
CHECKER = ROOT / "scripts" / "check_m262_b002_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation.py"
TEST = ROOT / "tests" / "tooling" / "test_check_m262_b002_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation.py"


def run(cmd: list[str]) -> None:
    subprocess.run(cmd, cwd=ROOT, check=True)


def main() -> int:
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m262-b002-lane-b-readiness",
        "--summary-out",
        "tmp/reports/m262/M262-B002/ensure_objc3c_native_build_summary.json",
    ])
    run([sys.executable, str(CHECKER)])
    run([sys.executable, "-m", "pytest", str(TEST), "-q"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

