#!/usr/bin/env python3
"""Run M270-C002 lane-C readiness checks without deep npm nesting."""

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
    print("[info] dependency continuity token: M270-C001 + M270-C002 (actor lowering metadata now feeds live helper-backed actor thunk/hop/nonisolated rewrites while broader mailbox/runtime closure remains later M270 work)")
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, "scripts/check_m270_c002_actor_thunk_hop_and_isolation_lowering_core_feature_implementation.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m270_c002_actor_thunk_hop_and_isolation_lowering_core_feature_implementation.py", "-q"])


if __name__ == "__main__":
    main()
