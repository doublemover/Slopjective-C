from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = (
    ROOT
    / "scripts"
    / "check_m266_d001_cleanup_and_unwind_integration_contract_and_architecture_freeze.py"
)
SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m266"
    / "M266-D001"
    / "pytest_static_summary.json"
)
CONTRACT_ID = "objc3c-part5-cleanup-unwind-integration-freeze/m266-d001-v1"


def test_m266_d001_static_bundle_contract_check_runs_truthfully() -> None:
    completed = subprocess.run(
        [
            sys.executable,
            str(CHECKER),
            "--skip-dynamic-probes",
            "--summary-out",
            str(SUMMARY),
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["dynamic_probes_executed"] is False
