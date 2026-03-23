#!/usr/bin/env python3
"""Lane-D readiness runner for M268-D001."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
BUILD_HELPER = ROOT / "scripts" / "ensure_objc3c_native_build.py"
DEPENDENCY_TOKEN = "M268-C003 -> M268-D001"

COMMAND_CHAIN: tuple[Sequence[str], ...] = (
    (
        sys.executable,
        str(BUILD_HELPER),
        "--mode",
        "fast",
        "--reason",
        "m268-d001-lane-d-readiness",
        "--summary-out",
        "tmp/reports/m268/M268-D001/ensure_objc3c_native_build_summary.json",
    ),
    (sys.executable, "scripts/build_objc3c_native_docs.py"),
    (
        sys.executable,
        "scripts/check_m268_d001_continuation_and_runtime_helper_contract_and_architecture_freeze.py",
    ),
    (
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m268_d001_continuation_and_runtime_helper_contract_and_architecture_freeze.py",
        "-q",
    ),
)


def main() -> int:
    print(
        f"[info] dependency continuity token: {DEPENDENCY_TOKEN} (lane D freezes a real private continuation helper ABI and testing snapshot while the live async lowering slice remains direct-call only)",
        flush=True,
    )
    for command in COMMAND_CHAIN:
        text = " ".join(command)
        print(f"[run] {text}", flush=True)
        completed = subprocess.run(command, cwd=ROOT, check=False)
        if completed.returncode != 0:
            print(
                f"[error] command failed with exit code {completed.returncode}: {text}",
                file=sys.stderr,
            )
            return completed.returncode
    print("[ok] M268-D001 lane-D readiness chain completed", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
