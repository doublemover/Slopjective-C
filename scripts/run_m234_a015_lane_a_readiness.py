#!/usr/bin/env python3
"""Run M234-A015 lane-A readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

BASELINE_DEPENDENCIES = ("M234-A014",)
PENDING_SEEDED_DEPENDENCY_TOKENS: tuple[str, ...] = ()

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/run_m234_a014_lane_a_readiness.py",
    ),
    (
        sys.executable,
        "scripts/check_m234_a015_property_and_ivar_syntax_surface_completion_advanced_core_workpack_shard_1_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m234_a015_property_and_ivar_syntax_surface_completion_advanced_core_workpack_shard_1_contract.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(f"[info] baseline dependencies: {', '.join(BASELINE_DEPENDENCIES)}")
    if PENDING_SEEDED_DEPENDENCY_TOKENS:
        print(
            "[info] pending seeded dependency tokens: "
            f"{', '.join(PENDING_SEEDED_DEPENDENCY_TOKENS)}"
        )
    else:
        print("[info] pending seeded dependency tokens: none")
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
    print("[ok] M234-A015 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())


