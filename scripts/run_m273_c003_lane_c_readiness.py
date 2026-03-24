#!/usr/bin/env python3
"""Lane-C readiness runner for M273-C003."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
DEPENDENCY_TOKEN = "M273-C001 + M273-C002 + M273-C003"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (
        sys.executable,
        "scripts/check_m273_c003_module_interface_and_replay_preservation_completion_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m273_c003_module_interface_and_replay_preservation_completion_core_feature_expansion.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        f"[info] dependency continuity token: {DEPENDENCY_TOKEN} "
        "M273-C003 module/interface replay preservation completion"
    )
    for command in COMMAND_CHAIN:
        text = " ".join(command)
        print(f"[run] {text}")
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            print(f"[error] command failed with exit code {completed.returncode}: {text}", file=sys.stderr)
            return completed.returncode
    print("[ok] M273-C003 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
