#!/usr/bin/env python3
"""Run M269-D003 lane-D readiness checks without deep npm nesting."""

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
    print("[info] dependency continuity token: M269-D002 + M269-D003 (live Part 7 task runtime is now hardened across cancellation, autoreleasepool, and reset-stable replay edges while lane-E closeout remains M269-E001)")
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, "scripts/check_m269_d003_cancellation_and_autorelease_integration_hardening_edge_case_and_compatibility_completion.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m269_d003_cancellation_and_autorelease_integration_hardening_edge_case_and_compatibility_completion.py", "-q"])


if __name__ == "__main__":
    main()
