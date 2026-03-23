#!/usr/bin/env python3
"""Lane-A readiness for M271-A003."""

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
    run([sys.executable, "scripts/check_m271_a002_frontend_cleanup_resource_and_capture_surface_completion_core_feature_implementation.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m271_a002_frontend_cleanup_resource_and_capture_surface_completion_core_feature_implementation.py", "-q"])
    run([sys.executable, "scripts/check_m271_a003_retainable_c_family_declaration_surface_core_feature_expansion.py"])
    run([sys.executable, "-m", "pytest", "tests/tooling/test_check_m271_a003_retainable_c_family_declaration_surface_core_feature_expansion.py", "-q"])
    print("[info] dependency continuity token: M271-A002 + M271-A003 (Part 8 frontend now admits retainable-family callable annotations and compatibility aliases before later M271 legality and runtime work)")
    print("[ok] M271-A003 lane-A readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
