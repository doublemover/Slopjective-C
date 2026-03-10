from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = (
    ROOT
    / "scripts"
    / "check_m257_d001_runtime_property_and_layout_consumption_contract_and_architecture_freeze.py"
)
SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "m257"
    / "M257-D001"
    / "property_layout_runtime_contract_summary.json"
)
CONTRACT_ID = "objc3c-runtime-property-layout-consumption-freeze/m257-d001-v1"


def test_m257_d001_checker_static_contract() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["dynamic_probes_executed"] is False
