#!/usr/bin/env python3
"""Run M270-D003 lane-D readiness checks without deep npm nesting."""

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
    print("[info] dependency continuity token: M270-D002 + M270-D003 (actor mailbox/isolation replay facts now survive runtime-import surfaces and mixed-module link plans while lane-E closeout remains later M270 work)")
    run([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run([sys.executable, "scripts/check_m270_d003_cross_module_isolation_metadata_hardening_edge_case_and_compatibility_completion.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m270_d003_cross_module_isolation_metadata_hardening_edge_case_and_compatibility_completion.py", "-q"])


if __name__ == "__main__":
    main()
