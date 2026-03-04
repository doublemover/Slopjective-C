#!/usr/bin/env python3
"""Run M249-E015 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import os
import subprocess
import sys
from pathlib import Path
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"


COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/run_m249_e014_lane_e_readiness.py",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "check:objc3c:m249-a006-lane-a-readiness",
    ),
    (
        sys.executable,
        "scripts/run_m249_b007_lane_b_readiness.py",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "check:objc3c:m249-c008-lane-c-readiness",
    ),
    (
        sys.executable,
        "scripts/run_m249_d015_lane_d_readiness.py",
    ),
    (
        sys.executable,
        "scripts/check_m249_e015_lane_e_release_gate_docs_and_runbooks_advanced_core_workpack_shard1_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m249_e015_lane_e_release_gate_docs_and_runbooks_advanced_core_workpack_shard1_contract.py",
        "-q",
    ),
)


def build_subprocess_env() -> dict[str, str]:
    env = os.environ.copy()
    shim_dir = Path("tmp") / "tool-shims"
    shim_dir.mkdir(parents=True, exist_ok=True)
    if sys.platform == "win32":
        shim_path = shim_dir / "python.cmd"
        shim_path.write_text(f'@echo off\r\n"{sys.executable}" %*\r\n', encoding="utf-8")
    else:
        shim_path = shim_dir / "python"
        shim_path.write_text(f'#!/usr/bin/env sh\nexec "{sys.executable}" "$@"\n', encoding="utf-8")
        shim_path.chmod(0o755)
    env["PATH"] = str(shim_dir.resolve()) + os.pathsep + env.get("PATH", "")
    return env


def run_chain() -> int:
    env = build_subprocess_env()
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}")
        completed = subprocess.run(command, check=False, env=env)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {command_text}",
                file=sys.stderr,
            )
            return completed.returncode
    print("[ok] M249-E015 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
