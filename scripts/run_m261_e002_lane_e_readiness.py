#!/usr/bin/env python3
"""Run the M261-E002 closeout readiness chain."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

DEPENDENCY_TOKEN = "M261-A003 + M261-B003 + M261-C004 + M261-D003 + M261-E001"
NPM = "npm.cmd" if sys.platform == "win32" else "npm"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (NPM, "run", "build:objc3c-native"),
    (
        sys.executable,
        "scripts/check_m261_a003_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "scripts/check_m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "scripts/check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py",
        "--skip-dynamic-probes",
    ),
    (
        sys.executable,
        "scripts/check_m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "scripts/check_m261_e001_runnable_block_runtime_gate_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "scripts/check_m261_e002_runnable_block_execution_matrix_for_captures_byref_helpers_and_escaping_blocks.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m261_e002_runnable_block_execution_matrix_for_captures_byref_helpers_and_escaping_blocks.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (lane-E closeout aligns the retained source, sema, lowering, runtime, and gate evidence with one executable matrix before M261 hands off to M262)",
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
    print("[ok] M261-E002 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
