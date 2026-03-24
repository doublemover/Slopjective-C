from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m274_e001_interop_conformance_gate_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m274" / "M274-E001" / "interop_conformance_gate_summary.json"


def test_m274_e001_checker_writes_static_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-part11-interop-conformance-gate/m274-e001-v1"
    assert payload["mode"] == "m274-e001-part11-interop-conformance-gate-v1"
    assert payload["next_closeout_issue"] == "M274-E002"
    assert payload["dynamic"]["skipped"] is True
    assert payload["upstream"]["M274-D002"]["contract_id"] == "objc3c-part11-header-module-and-bridge-generation/m274-d002-v1"
    assert payload["ok"] is True


def test_m274_e001_checker_writes_dynamic_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["dynamic"]["skipped"] is False
    assert payload["d002_runtime_boundary"]["runtime_probe"]["bridge_generation_ready"] == "1"
    assert payload["d002_runtime_boundary"]["consumer_link_plan"]["part11_header_module_bridge_cross_module_packaging_ready"] is True
    assert payload["ok"] is True
