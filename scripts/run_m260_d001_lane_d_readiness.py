#!/usr/bin/env python3
"""Run the focused M260-D001 lane-D readiness stack."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M260-C002 -> M260-D001"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (NPM_EXECUTABLE, "run", "build:objc3c-native"),
    (
        sys.executable,
        "scripts/check_m260_c002_runtime_hook_emission_for_retain_release_autorelease_and_weak_paths_core_feature_implementation.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m260_d001_runtime_memory_management_api_contract_and_architecture_freeze.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m260_d001_runtime_memory_management_api_contract_and_architecture_freeze.py",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (runtime API freeze stays aligned with the live C002 helper surface while only rerunning the directly affected lane-D stack)"
    )
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}")
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {command_text}",
                file=sys.stderr,
            )
            return completed.returncode
    print("[ok] M260-D001 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
