#!/usr/bin/env python3
"""Run M235-A014 lane-A readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence


COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/run_m235_a013_lane_a_readiness.py",
    ),
    (
        sys.executable,
        "scripts/check_m235_a014_qualifier_and_generic_grammar_normalization_release_candidate_and_replay_dry_run_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m235_a014_qualifier_and_generic_grammar_normalization_release_candidate_and_replay_dry_run_contract.py",
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
    print("[ok] M235-A014 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
