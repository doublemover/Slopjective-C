from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SUMMARY = ROOT / "tmp" / "reports" / "m271" / "M271-D002" / "live_cleanup_retainable_runtime_integration_summary.json"


def test_m271_d002_checker_passes() -> None:
    completed = subprocess.run(
        [
            sys.executable,
            "scripts/check_m271_d002_live_cleanup_helpers_and_retainable_family_integration_core_feature_implementation.py",
            "--skip-dynamic-probes",
            "--summary-out",
            str(SUMMARY),
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["mode"] == "m271-d002-live-cleanup-retainable-runtime-integration-v1"
    assert payload["contract_id"] == "objc3c-part8-live-cleanup-retainable-runtime-integration/m271-d002-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
