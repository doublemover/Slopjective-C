#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    completed = subprocess.run(command, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M267-B001 + M267-B002 (Part 6 now admits try/throw/do-catch in source-only sema while native lowering/runtime remain deferred)")
    run([
        sys.executable,
        "scripts/check_m267_b002_try_do_catch_and_propagation_semantics_core_feature_implementation.py",
    ])
    run([
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m267_b002_try_do_catch_and_propagation_semantics_core_feature_implementation.py",
        "-q",
    ])
    print("[ok] M267-B002 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
