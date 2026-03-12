from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m262_c002_automatic_retain_release_autorelease_insertion_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m262" / "M262-C002" / "arc_automatic_insertion_pytest_summary.json"
CONTRACT_ID = "objc3c-arc-automatic-insertion/m262-c002-v1"


def test_checker_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes", "--summary-out", str(SUMMARY)],
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
