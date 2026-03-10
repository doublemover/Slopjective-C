from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m259_a001_runnable_sample_surface_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m259" / "M259-A001" / "runnable_sample_surface_contract_summary.json"
CONTRACT_ID = "objc3c-runnable-sample-surface/m259-a001-v1"


def test_checker_passes(tmp_path: Path) -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=str(ROOT),
        check=False,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr


def test_summary_contains_expected_surface() -> None:
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["next_issue"] == "M259-A002"
    assert payload["sample_surface"]["positive_fixture_count"] > 0
    assert payload["sample_surface"]["negative_fixture_count"] > 0
    assert payload["upstream_summaries"]["M256-D004"]["sample_exit_code"] == 37
    assert payload["upstream_summaries"]["M257-E002"]["ok"] is True
    assert payload["upstream_summaries"]["M258-E002"]["startup_registered_image_count"] == 2
