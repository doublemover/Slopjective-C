#!/usr/bin/env python3
"""Run M257-B002 lane-B readiness while preserving the live summary."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M257-B002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "build:objc3c-native"),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m257_b001_property_and_ivar_executable_semantics_contract_and_architecture_freeze.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m257_b001_property_and_ivar_executable_semantics_contract_and_architecture_freeze.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m257_b002_property_synthesis_and_default_ivar_binding_full_semantics_core_feature_implementation.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m257_b002_property_synthesis_and_default_ivar_binding_full_semantics_core_feature_implementation.py",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (native build is refreshed first, then the B001 freeze is replayed before the live B002 proof rewrites the final summary)",
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
    print("[ok] M257-B002 lane-B readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
