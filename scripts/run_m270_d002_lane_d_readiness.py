#!/usr/bin/env python3
"""Run M270-D002 lane-D readiness checks without deep npm nesting."""

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
    print("[info] dependency continuity token: M270-D001 + M270-D002 (the private actor helper slice now proves live mailbox bind/enqueue/drain behavior through the packaged runtime while broader cross-module actor metadata hardening remains later M270 work)")
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, "scripts/check_m270_d002_live_actor_mailbox_and_isolation_runtime_implementation_core_feature_implementation.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m270_d002_live_actor_mailbox_and_isolation_runtime_implementation_core_feature_implementation.py", "-q"])


if __name__ == "__main__":
    main()




