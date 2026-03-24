from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m275_d001_ci_runbook_and_dashboard_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m275" / "M275-D001" / "pytest_summary.json"


def test_checker_static_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes", "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["contract_id"] == "objc3c-advanced-feature-ci-runbook-dashboard-contract/m275-d001-v1"


def test_checker_summary_records_next_issue() -> None:
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["next_issue"] == "M275-D002"
