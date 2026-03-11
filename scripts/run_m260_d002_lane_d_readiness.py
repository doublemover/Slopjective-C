"""Run the M260-D002 lane-D readiness chain without recursive npm nesting."""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
DEPENDENCY_TOKEN = "M260-C002 + M260-D001 + M260-D002"


def run_command(command: Sequence[str]) -> None:
    result = subprocess.run(
        list(command),
        cwd=ROOT,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if result.returncode != 0:
        raise SystemExit(result.returncode)


def main() -> int:
    print(f"[info] dependency_token={DEPENDENCY_TOKEN}")
    run_command([sys.executable, "scripts/build_objc3c_native_docs.py"])
    run_command([sys.executable, "scripts/check_m260_c002_runtime_hook_emission_for_retain_release_autorelease_and_weak_paths_core_feature_implementation.py", "--skip-dynamic-probes"])
    run_command([sys.executable, "scripts/check_m260_d001_runtime_memory_management_api_contract_and_architecture_freeze.py", "--skip-dynamic-probes"])
    run_command([sys.executable, "-m", "pytest", "tests/tooling/test_check_m260_d002_reference_counting_weak_table_and_autoreleasepool_implementation_core_feature_implementation.py", "-q"])
    run_command([sys.executable, "scripts/check_m260_d002_reference_counting_weak_table_and_autoreleasepool_implementation_core_feature_implementation.py"])
    print("[ok] M260-D002 lane-D readiness chain completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
