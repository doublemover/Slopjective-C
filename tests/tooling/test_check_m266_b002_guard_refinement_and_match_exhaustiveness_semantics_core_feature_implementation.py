from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m266_b002_guard_refinement_and_match_exhaustiveness_semantics_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-B002" / "pytest_static_summary.json"


def test_m266_b002_static_contract_check_passes() -> None:
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
    assert payload["contract_id"] == "objc3c-part5-control-flow-semantic-model/m266-b001-v1"
