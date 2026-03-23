#!/usr/bin/env python3
"""Lane-C readiness runner for M267-C002."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DEPENDENCY_TOKEN = "M267-B002 + M267-B003 + M267-C002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m267-c002-lane-c-readiness",
        "--summary-out",
        "tmp/reports/m267/M267-C002/ensure_objc3c_native_build_summary.json",
    ),
    (sys.executable, "scripts/check_m267_b002_try_do_catch_and_propagation_semantics_core_feature_implementation.py", "--skip-dynamic-probes"),
    (sys.executable, "scripts/check_m267_b003_bridging_legality_and_diagnostic_completion_edge_case_and_compatibility_completion.py", "--skip-dynamic-probes"),
    (sys.executable, "scripts/check_m267_c002_error_out_abi_and_propagation_lowering_core_feature_implementation.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m267_c002_error_out_abi_and_propagation_lowering_core_feature_implementation.py", "-q"),
)


def run_chain() -> int:
    print(f"[info] dependency continuity token: {DEPENDENCY_TOKEN} (lane C now proves runnable error-out ABI, propagation, and do/catch lowering)")
    for command in COMMAND_CHAIN:
        text = " ".join(command)
        print(f"[run] {text}")
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            print(f"[error] command failed with exit code {completed.returncode}: {text}", file=sys.stderr)
            return completed.returncode
    print("[ok] M267-C002 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
