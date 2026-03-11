#!/usr/bin/env python3
"""Run the focused M261-B002 lane-B readiness chain."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CHECKER = ROOT / "scripts" / "check_m261_b002_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation.py"
TEST = ROOT / "tests" / "tooling" / "test_check_m261_b002_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation.py"
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"


def run(cmd: list[str]) -> None:
    subprocess.run(cmd, cwd=ROOT, check=True)


def main() -> int:
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([NPM_EXECUTABLE, "run", "build:objc3c-native"])
    run([sys.executable, str(CHECKER)])
    run([sys.executable, "-m", "pytest", str(TEST), "-q"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
