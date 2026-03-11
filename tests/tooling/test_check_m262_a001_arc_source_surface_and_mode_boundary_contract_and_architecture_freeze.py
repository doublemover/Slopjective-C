from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m262_a001_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze.py"
TEST_SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-A001" / "arc_source_mode_boundary_summary.test.json"
CONTRACT_ID = "objc3c-arc-source-mode-boundary-freeze/m262-a001-v1"


def test_m262_a001_checker_emits_summary() -> None:
    completed = subprocess.run(
        [
            sys.executable,
            str(SCRIPT),
            "--skip-dynamic-probes",
            "--summary-out",
            str(TEST_SUMMARY),
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(TEST_SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["dynamic_probes_executed"] is False
    assert payload["dynamic_case"]["skipped"] is True
    assert payload["next_issue"] == "M262-A002"
