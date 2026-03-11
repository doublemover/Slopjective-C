#!/usr/bin/env python3
"""Run the focused M259-D003 lane-D readiness stack."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
D002_CHECKER = ROOT / "scripts" / "check_m259_d002_build_install_run_workflow_and_binary_packaging_core_feature_implementation.py"
D003_CHECKER = ROOT / "scripts" / "check_m259_d003_platform_prerequisites_and_runtime_bring_up_documentation_core_feature_expansion.py"
D003_TEST = ROOT / "tests" / "tooling" / "test_check_m259_d003_platform_prerequisites_and_runtime_bring_up_documentation_core_feature_expansion.py"


def run(cmd: list[str]) -> None:
    subprocess.run(cmd, cwd=ROOT, check=True)


def main() -> int:
    run([sys.executable, str(D002_CHECKER), "--skip-dynamic-probes"])
    run([sys.executable, "-m", "pytest", str(D003_TEST), "-q"])
    run([sys.executable, str(D003_CHECKER)])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
