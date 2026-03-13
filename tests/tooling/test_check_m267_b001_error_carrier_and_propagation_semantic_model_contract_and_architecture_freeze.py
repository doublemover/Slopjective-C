from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m267_b001_error_carrier_and_propagation_semantic_model_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m267" / "M267-B001" / "pytest_static_summary.json"


def test_m267_b001_static_contract_check_passes() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes", "--summary-out", str(SUMMARY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["checks_failed"] == 0
    assert payload["contract_id"] == "objc3c-part6-error-semantic-model/m267-b001-v1"
