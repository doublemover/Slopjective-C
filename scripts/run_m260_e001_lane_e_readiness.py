#!/usr/bin/env python3
"""Run the lean M260-E001 lane-E readiness chain."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

DEPENDENCY_TOKEN = "M260-C002 + M260-D001 + M260-D002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m260_c002_runtime_hook_emission_for_retain_release_autorelease_and_weak_paths_core_feature_implementation.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "scripts/check_m260_d001_runtime_memory_management_api_contract_and_architecture_freeze.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "scripts/check_m260_e001_ownership_runtime_gate_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m260_e001_ownership_runtime_gate_contract_and_architecture_freeze.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (lane-E freezes the already-proven ownership hook, memory API, and runtime implementation baseline before E002 expands runnable smoke)",
        flush=True,
    )
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}", flush=True)
        completed = subprocess.run(command, check=False)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {command_text}",
                file=sys.stderr,
                flush=True,
            )
            return completed.returncode
    print("[ok] M260-E001 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
