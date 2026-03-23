from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SUMMARY = ROOT / "tmp" / "reports" / "m267" / "M267-D002" / "live_error_runtime_integration_pytest_summary.json"


def test_m267_d002_checker_passes() -> None:
    completed = subprocess.run(
        [
            sys.executable,
            "scripts/check_m267_d002_live_catch_bridge_and_runtime_integration_core_feature_implementation.py",
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
    assert payload["mode"] == "m267-d002-live-catch-bridge-and-runtime-integration-core-feature-implementation-v1"
    assert payload["contract_id"] == "objc3c-part6-live-error-runtime-integration/m267-d002-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
