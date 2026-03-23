#!/usr/bin/env python3
"""Run M270-C003 lane-C readiness checks without deep npm nesting."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> None:
    print("[info] dependency continuity token: M270-C002 + M270-C003 (actor lowering now carries replay-proof and race-guard helper-backed runtime artifacts while broader lane-D actor runtime closure remains later M270 work)")
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, "scripts/check_m270_c003_replay_proof_and_race_guard_artifact_integration_core_feature_expansion.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m270_c003_replay_proof_and_race_guard_artifact_integration_core_feature_expansion.py", "-q"])


if __name__ == "__main__":
    main()
