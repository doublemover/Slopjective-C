#!/usr/bin/env python3
"""Run M256-E001 lane-E readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from typing import Sequence

NPM_EXECUTABLE = "npm.cmd" if sys.platform == "win32" else "npm"
DEPENDENCY_TOKEN = "M256-A003 + M256-B004 + M256-C003 + M256-D004"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (NPM_EXECUTABLE, "run", "check:objc3c:m256-a003-lane-a-readiness"),
    (
        sys.executable,
        "scripts/check_m256_b004_inheritance_override_and_realization_legality_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m256_b004_inheritance_override_and_realization_legality_core_feature_expansion.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m256_c003_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m256_c003_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m256_d004_canonical_runnable_class_and_object_sample_support_core_feature_expansion.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m256_d004_canonical_runnable_class_and_object_sample_support_core_feature_expansion.py",
        "-q",
    ),
    (
        sys.executable,
        "scripts/check_m256_e001_class_protocol_and_category_conformance_gate_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m256_e001_class_protocol_and_category_conformance_gate_contract_and_architecture_freeze.py",
        "-q",
    ),
)


def run_chain() -> int:
    print(
        "[info] dependency continuity token: "
        f"{DEPENDENCY_TOKEN} (source closure replays once, then inheritance legality, realization records, and canonical runnable object evidence are checked directly without recursive lane nesting)",
        flush=True,
    )
    for command in COMMAND_CHAIN:
        command_text = " ".join(command)
        print(f"[run] {command_text}", flush=True)
        completed = subprocess.run(command, check=False)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {command_text}",
                file=sys.stderr,
                flush=True,
            )
            return completed.returncode
    print("[ok] M256-E001 lane-E readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(run_chain())
