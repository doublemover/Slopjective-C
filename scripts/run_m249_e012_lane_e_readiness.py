#!/usr/bin/env python3
"""Run M249-E012 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import argparse
import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"


FULL_COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/run_m249_e011_lane_e_readiness.py",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "check:objc3c:m249-a009-lane-a-readiness",
    ),
    (
        sys.executable,
        "scripts/run_m249_b011_lane_b_readiness.py",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "check:objc3c:m249-c012-lane-c-readiness",
    ),
    (
        sys.executable,
        "scripts/run_m249_d012_lane_d_readiness.py",
    ),
    (
        sys.executable,
        "scripts/check_m249_e012_lane_e_release_gate_docs_and_runbooks_cross_lane_integration_sync_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m249_e012_lane_e_release_gate_docs_and_runbooks_cross_lane_integration_sync_contract.py",
        "-q",
    ),
)

LOCAL_COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m249_e012_lane_e_release_gate_docs_and_runbooks_cross_lane_integration_sync_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m249_e012_lane_e_release_gate_docs_and_runbooks_cross_lane_integration_sync_contract.py",
        "-q",
    ),
)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--local-only",
        action="store_true",
        help="Run only local E012 contract and pytest checks (skip upstream lane dependencies).",
    )
    return parser.parse_args(argv)


def run_chain(command_chain: tuple[Sequence[str], ...], *, ok_message: str) -> int:
    for command in command_chain:
        command_text = " ".join(command)
        print(f"[run] {command_text}")
        completed = subprocess.run(command, check=False)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {command_text}",
                file=sys.stderr,
            )
            return completed.returncode
    print(ok_message)
    return 0


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    if args.local_only:
        return run_chain(
            LOCAL_COMMAND_CHAIN,
            ok_message="[ok] M249-E012 lane-E local readiness chain completed",
        )
    return run_chain(
        FULL_COMMAND_CHAIN,
        ok_message="[ok] M249-E012 lane-E readiness chain completed",
    )


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
