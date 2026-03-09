#!/usr/bin/env python3
"""Run the M256-B003 lane-B readiness chain without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M256-A003 + M256-B001 + M256-B002"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "check:objc3c:m256-a003-lane-a-readiness"),
    (NPM_EXECUTABLE, "run", "build:objc3c-native"),
    (
        sys.executable,
        "scripts/check_m256_b001_object_model_semantic_rules_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m256_b001_object_model_semantic_rules_contract_and_architecture_freeze.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m256_b002_protocol_conformance_and_required_optional_member_enforcement_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m256_b002_protocol_conformance_and_required_optional_member_enforcement_core_feature_implementation.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m256_b003_category_merge_and_conflict_semantics_core_feature_implementation.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m256_b003_category_merge_and_conflict_semantics_core_feature_implementation.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (source closure, object-model freeze, conformance enforcement, and realized-class category merge legality)"
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
    print("[ok] M256-B003 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
