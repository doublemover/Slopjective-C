from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m266_d002_runtime_cleanup_and_unwind_integration_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m266" / "M266-D002" / "runtime_cleanup_and_unwind_integration_summary.test.json"
CONTRACT_ID = "objc3c-runtime-cleanup-unwind-integration/m266-d002-v1"


def test_m266_d002_checker_emits_summary() -> None:
    result = subprocess.run(
        [
            sys.executable,
            str(CHECKER),
            "--skip-dynamic-probes",
            "--summary-out",
            str(SUMMARY),
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["issue"] == "M266-D002"
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["next_issue"] == "M266-E001"
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []
