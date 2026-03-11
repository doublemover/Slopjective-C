#!/usr/bin/env python3
"""Run the focused M260-C002 lane-C readiness stack."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

ROOT = __import__("pathlib").Path(__file__).resolve().parents[1]
NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M260-C001 -> M260-C002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (NPM_EXECUTABLE, "run", "build:objc3c-native"),
    (
        sys.executable,
        "scripts/check_m260_c001_ownership_lowering_baseline_contract_and_architecture_freeze.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m260_c002_runtime_hook_emission_for_retain_release_autorelease_and_weak_paths_core_feature_implementation.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m260_c002_runtime_hook_emission_for_retain_release_autorelease_and_weak_paths_core_feature_implementation.py",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (ownership runtime hook emission stays aligned with the frozen M260-C001 boundary while only rerunning the directly affected lane-C stack)"
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
    print("[ok] M260-C002 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
