from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m257_e002_runnable_property_ivar_and_accessor_conformance_plus_samples_cross_lane_integration_sync.py"
SUMMARY = ROOT / "tmp" / "reports" / "m257" / "M257-E002" / "runnable_property_ivar_execution_matrix_summary.json"
CONTRACT_ID = "objc3c-runnable-property-ivar-accessor-execution-matrix/m257-e002-v1"


def test_checker_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["dynamic_probes_executed"] is False
    assert payload["next_issue"] == "M258-A001"
    assert payload["upstream_summaries"]["M257-E001"]["contract_id"] == "objc3c-executable-property-ivar-execution-gate/m257-e001-v1"
