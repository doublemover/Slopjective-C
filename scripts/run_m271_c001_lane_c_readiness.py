#!/usr/bin/env python3
"""Lane-C readiness for M271-C001."""

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
    run([sys.executable, "scripts/check_m271_b004_capture_list_and_retainable_family_legality_completion_edge_case_and_compatibility_completion.py", "--skip-dynamic-probes"])
    run([sys.executable, "scripts/check_m271_c001_system_extension_lowering_contract_and_architecture_freeze.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m271_c001_system_extension_lowering_contract_and_architecture_freeze.py", "-q"])
    print("[info] dependency continuity token: M271-B004 + M271-C001 (cleanup/resource, borrowed, and retainable-family legality now feed one truthful Part 8 lowering contract before later runtime interop work)")
    print("[ok] M271-C001 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
