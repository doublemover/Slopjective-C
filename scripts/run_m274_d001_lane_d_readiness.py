#!/usr/bin/env python3
"""Run M274-D001 lane-D readiness without npm nesting."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run(args: list[str]) -> None:
    completed = subprocess.run(args, cwd=ROOT, check=False)
    if completed.returncode != 0:
        raise SystemExit(completed.returncode)


def main() -> int:
    print("[info] dependency continuity token: M274-C002 + M274-C003 + M274-D001 (Part 11 now freezes one truthful packaging/toolchain boundary over the packaged runtime archive, registration manifest, cross-module link plan, and operator-visible evidence while live bridge generation remains later D002 work)")
    run([sys.executable, str(ROOT / "scripts" / "build_objc3c_native_docs.py")])
    run([sys.executable, str(ROOT / "scripts" / "check_m274_d001_bridge_packaging_and_toolchain_contract_and_architecture_freeze.py")])
    run([sys.executable, "-m", "pytest", str(ROOT / "tests" / "tooling" / "test_check_m274_d001_bridge_packaging_and_toolchain_contract_and_architecture_freeze.py"), "-q"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())