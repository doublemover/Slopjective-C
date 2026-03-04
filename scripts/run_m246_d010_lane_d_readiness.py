#!/usr/bin/env python3
"""Run M246-D010 lane-D readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence


DEPENDENCY_TOKEN = "M246-D009"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/run_m246_d009_lane_d_readiness.py",
    ),
    (
        sys.executable,
        "scripts/check_m246_d010_toolchain_integration_and_optimization_controls_conformance_corpus_expansion_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m246_d010_toolchain_integration_and_optimization_controls_conformance_corpus_expansion_contract.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (inherits lane-D conformance matrix implementation readiness)"
    )
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
    print("[ok] M246-D010 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
