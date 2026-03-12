#!/usr/bin/env python3
"""Run the focused M262-C002 lane-C readiness chain."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DEPENDENCY_TOKEN = "M262-A001..A002 + M262-B001..B003 + M262-C001"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m262-c002-lane-c-readiness",
        "--summary-out",
        "tmp/reports/m262/M262-C002/ensure_objc3c_native_build_summary.json",
    ),
    (sys.executable, "scripts/check_m262_a001_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze.py", "--skip-dynamic-probes"),
    (sys.executable, "scripts/check_m262_a002_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation.py", "--skip-dynamic-probes"),
    (sys.executable, "scripts/check_m262_b001_arc_semantic_rules_and_forbidden_forms_contract_and_architecture_freeze.py", "--skip-dynamic-probes"),
    (sys.executable, "scripts/check_m262_b002_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation.py", "--skip-dynamic-probes"),
    (sys.executable, "scripts/check_m262_b003_weak_autorelease_property_synthesis_and_block_interaction_arc_semantics_core_feature_expansion.py", "--skip-dynamic-probes"),
    (sys.executable, "scripts/check_m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze.py", "--skip-dynamic-probes"),
    (sys.executable, "scripts/check_m262_c002_automatic_retain_release_autorelease_insertion_core_feature_implementation.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m262_c002_automatic_retain_release_autorelease_insertion_core_feature_implementation.py", "-q"),
)


def run_chain() -> int:
    print(f"[info] dependency continuity token: {DEPENDENCY_TOKEN} (lane C now consumes the frozen ARC semantic insertion packets and lowering boundary to emit real retain/release/autorelease helper calls)")
    for command in COMMAND_CHAIN:
        text = " ".join(command)
        print(f"[run] {text}")
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            print(f"[error] command failed with exit code {completed.returncode}: {text}", file=sys.stderr)
            return completed.returncode
    print("[ok] M262-C002 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
