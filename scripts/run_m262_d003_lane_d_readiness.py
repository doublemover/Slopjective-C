#!/usr/bin/env python3
"""Lane-D readiness runner for M262-D003."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DEPENDENCY_TOKEN = "M260-D002 + M262-C004 + M262-D002 + M262-D003"

COMMANDS: tuple[tuple[str, ...], ...] = (
    (sys.executable, "scripts/check_m260_d002_reference_counting_weak_table_and_autoreleasepool_implementation_core_feature_implementation.py", "--skip-dynamic-probes"),
    (sys.executable, "scripts/check_m262_c004_arc_and_block_interaction_lowering_with_autorelease_return_conventions_core_feature_expansion.py", "--skip-dynamic-probes"),
    (sys.executable, "scripts/check_m262_d002_arc_helper_entrypoints_weak_operations_and_autorelease_return_runtime_support_core_feature_implementation.py", "--skip-dynamic-probes"),
    (sys.executable, "scripts/check_m262_d003_ownership_debug_instrumentation_and_runtime_validation_hooks_for_arc_core_feature_expansion.py"),
    (sys.executable, "-m", "pytest", "tests/tooling/test_check_m262_d003_ownership_debug_instrumentation_and_runtime_validation_hooks_for_arc_core_feature_expansion.py", "-q"),
)


def run(command: tuple[str, ...]) -> None:
    subprocess.run(command, cwd=ROOT, check=True)


def main() -> int:
    print(
        f"[info] dependency continuity token: {DEPENDENCY_TOKEN} "
        "(lane D now proves private ARC debug counters and validation hooks "
        "above the retained ARC helper runtime support surface)",
        flush=True,
    )
    for command in COMMANDS:
        run(command)
    print("[ok] M262-D003 lane-D readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
