#!/usr/bin/env python3
"""Lane-D readiness chain for M255-D003."""

from __future__ import annotations

import os
import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
NPM = "npm.cmd" if os.name == "nt" else "npm"

COMMANDS: tuple[tuple[str, ...], ...] = (
    (NPM, "run", "build:objc3c-native"),
    ("python", "scripts/check_m255_d002_selector_interning_and_lookup_tables_core_feature_implementation.py"),
    (
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m255_d002_selector_interning_and_lookup_tables_core_feature_implementation.py",
        "-q",
    ),
    ("python", "scripts/check_m255_d003_method_cache_and_slow_path_lookup_core_feature_implementation.py"),
    (
        "python",
        "-m",
        "pytest",
        "tests/tooling/test_check_m255_d003_method_cache_and_slow_path_lookup_core_feature_implementation.py",
        "-q",
    ),
)


def run(command: Sequence[str]) -> int:
    print("[run]", " ".join(command), flush=True)
    completed = subprocess.run(command, cwd=ROOT, check=False)
    return int(completed.returncode)


def main() -> int:
    for command in COMMANDS:
        status = run(command)
        if status != 0:
            return status
    print("[ok] M255-D003 lane-D readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
