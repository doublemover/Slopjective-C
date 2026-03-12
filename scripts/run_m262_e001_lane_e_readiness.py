#!/usr/bin/env python3
"""Run the lean M262-E001 lane-E readiness chain."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

DEPENDENCY_TOKEN = "M262-A002 + M262-B003 + M262-C004 + M262-D003"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (
        sys.executable,
        "scripts/check_m262_a002_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "scripts/check_m262_b003_weak_autorelease_property_synthesis_and_block_interaction_arc_semantics_core_feature_expansion.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "scripts/check_m262_c004_arc_and_block_interaction_lowering_with_autorelease_return_conventions_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "scripts/check_m262_d003_ownership_debug_instrumentation_and_runtime_validation_hooks_for_arc_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "scripts/check_m262_e001_runnable_arc_runtime_gate_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m262_e001_runnable_arc_runtime_gate_contract_and_architecture_freeze.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (lane-E freezes the current runnable ARC proof chain before E002 broadens it into the ARC conformance matrix)",
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
    print("[ok] M262-E001 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
