from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m256_e002_runnable_class_protocol_and_category_execution_matrix_cross_lane_integration_sync.py"
CONTRACT_ID = "objc3c-runnable-class-protocol-category-execution-matrix/m256-e002-v1"
SUMMARY = ROOT / "tmp" / "reports" / "m256" / "M256-E002" / "runnable_class_protocol_category_execution_matrix_summary.json"


def test_checker_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    assert CONTRACT_ID in SUMMARY.read_text(encoding="utf-8")
