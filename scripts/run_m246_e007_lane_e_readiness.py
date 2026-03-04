#!/usr/bin/env python3
"""Run M246-E007 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"

BASELINE_DEPENDENCY = "M246-E006"
PENDING_SEEDED_DEPENDENCY_TOKENS = ("M246-B007", "M246-C013")

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        "scripts/check_m246_e006_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m246_e006_optimization_gate_and_perf_evidence_edge_case_expansion_and_robustness_contract.py",
        "-q",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m246-b007-lane-b-readiness",
    ),
    (
        NPM_EXECUTABLE,
        "run",
        "--if-present",
        "check:objc3c:m246-c013-lane-c-readiness",
    ),
    (
        sys.executable,
        "scripts/check_m246_e007_optimization_gate_and_perf_evidence_diagnostics_hardening_contract.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m246_e007_optimization_gate_and_perf_evidence_diagnostics_hardening_contract.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(f"[info] baseline dependency continuity token: {BASELINE_DEPENDENCY}")
    print(
        "[info] pending seeded dependency tokens: "
        f"{', '.join(PENDING_SEEDED_DEPENDENCY_TOKENS)}"
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
    print("[ok] M246-E007 lane-E readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
