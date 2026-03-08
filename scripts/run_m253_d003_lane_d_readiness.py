from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def run_step(cmd: list[str]) -> None:
    resolved = list(cmd)
    if resolved and resolved[0] == "npm" and sys.platform.startswith("win"):
        resolved[0] = "npm.cmd"
    result = subprocess.run(resolved, cwd=ROOT, text=True, encoding="utf-8")
    if result.returncode != 0:
        raise SystemExit(result.returncode)


if __name__ == "__main__":
    run_step(["npm", "run", "check:objc3c:m253-d002-lane-d-readiness"])
    run_step(["npm", "run", "build:objc3c-native"])
    run_step([
        sys.executable,
        "scripts/check_m253_d003_archive_and_static_link_metadata_discovery_behavior_edge_case_and_compatibility_completion.py",
    ])
    run_step([
        sys.executable,
        "-m",
        "pytest",
        "tests/tooling/test_check_m253_d003_archive_and_static_link_metadata_discovery_behavior_edge_case_and_compatibility_completion.py",
        "-q",
    ])
