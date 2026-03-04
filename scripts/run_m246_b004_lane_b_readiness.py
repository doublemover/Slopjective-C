#!/usr/bin/env python3
"""Run M246-B004 lane-B readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence


COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m246_b003_semantic_invariants_for_optimization_legality_core_feature_implementation_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m246_b003_semantic_invariants_for_optimization_legality_core_feature_implementation_contract.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m246_b004_semantic_invariants_for_optimization_legality_core_feature_expansion_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m246_b004_semantic_invariants_for_optimization_legality_core_feature_expansion_contract.py",
        "-q",
    ),
)


def run_chain() -> int:
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}")
        completed = subprocess.run(command, check=False)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {command_text}",
                file=sys.stderr,
            )
            return completed.returncode
    print("[ok] M246-B004 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())

