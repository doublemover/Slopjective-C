#!/usr/bin/env python3
"""Lane-C readiness for M271-C003."""

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
    run([
        sys.executable,
        "scripts/check_m271_b004_capture_list_and_retainable_family_legality_completion_edge_case_and_compatibility_completion.py",
        "--skip-dynamic-probes",
    ])
    run([
        sys.executable,
        "scripts/check_m271_c001_system_extension_lowering_contract_and_architecture_freeze.py",
        "--skip-dynamic-probes",
    ])
    run([
        sys.executable,
        "scripts/check_m271_c002_resource_cleanup_and_capture_lowering_core_feature_implementation.py",
        "--skip-dynamic-probes",
    ])
    run([
        sys.executable,
        "scripts/check_m271_c003_borrowed_pointer_and_retainable_family_abi_completion_core_feature_expansion.py",
    ])
    run([
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m271_c003_borrowed_pointer_and_retainable_family_abi_completion_core_feature_expansion.py",
        "-q",
    ])
    print("[info] dependency continuity token: M271-B004 + M271-C001 + M271-C002 + M271-C003 (Part 8 now publishes one frozen lowering contract plus a dedicated borrowed/retainable ABI replay packet for native artifact proof)")
    print("[ok] M271-C003 lane-C readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
