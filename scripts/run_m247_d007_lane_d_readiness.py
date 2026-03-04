#!/usr/bin/env python3
"""Run M247-D007 lane-D readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from shutil import which
from typing import Sequence


DEPENDENCY_TOKEN = "M247-D006"
NPM_COMMAND = which("npm") or which("npm.cmd") or "npm.cmd"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        NPM_COMMAND,
        "run",
        "check:objc3c:m247-d006-lane-d-readiness",
    ),
    (
        sys.executable,
        "scripts/check_m247_d007_runtime_link_build_throughput_optimization_diagnostics_hardening_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m247_d007_runtime_link_build_throughput_optimization_diagnostics_hardening_contract.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (inherits lane-D edge-case expansion and robustness readiness)"
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
    print("[ok] M247-D007 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
