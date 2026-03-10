#!/usr/bin/env python3
"""Run the M259-B002 lane-B readiness chain."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str]) -> None:
    print(f"[run] {' '.join(command)}")
    completed = subprocess.run(command, cwd=str(ROOT), check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print(
        "[info] dependency continuity token: M259-A002 + M259-B001 + M259-B002 "
        "(lane B turns the runnable-core compatibility guard into live fail-closed unsupported advanced-feature diagnostics before M259-C001 freezes replay/inspection)"
    )
    run([sys.executable, "scripts/check_m259_a002_canonical_runnable_sample_set.py", "--skip-dynamic-probes"])
    run([sys.executable, "scripts/check_m259_b001_runnable_core_compatibility_guard.py"])
    run([sys.executable, "scripts/check_m259_b002_fail_closed_unsupported_advanced_feature_diagnostics_core_feature_implementation.py"])
    run([
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m259_b002_fail_closed_unsupported_advanced_feature_diagnostics_core_feature_implementation.py",
        "-q",
    ])
    print("[ok] M259-B002 lane-B readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
