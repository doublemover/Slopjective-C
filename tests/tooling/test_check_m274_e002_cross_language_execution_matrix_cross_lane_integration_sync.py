from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m274_e002_cross_language_execution_matrix_cross_lane_integration_sync.py"
SUMMARY = ROOT / "tmp" / "reports" / "m274" / "M274-E002" / "cross_language_execution_matrix_summary.json"


def test_m274_e002_checker_writes_summary() -> None:
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
    assert payload["contract_id"] == "objc3c-part11-cross-language-execution-matrix/m274-e002-v1"
    assert payload["mode"] == "m274-e002-cross-language-execution-matrix-v1"
    assert payload["upstream_handoff_issue"] == "M274-E001"
    assert payload["next_issue"] == "M275-A001"
    assert payload["ok"] is True
    assert payload["dynamic"]["skipped"] is True


def test_m274_e002_checker_writes_dynamic_summary() -> None:
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
    assert payload["ok"] is True
    assert payload["dynamic"]["skipped"] is False
    assert payload["runnable_matrix"]["e002_executable_boundary"] == "M274-D002"
