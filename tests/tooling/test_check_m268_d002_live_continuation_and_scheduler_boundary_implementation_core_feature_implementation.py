from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SUMMARY = ROOT / "tmp" / "reports" / "m268" / "M268-D002" / "live_continuation_runtime_integration_summary.json"


def test_m268_d002_checker_passes() -> None:
    completed = subprocess.run(
        [
            sys.executable,
            "scripts/check_m268_d002_live_continuation_and_scheduler_boundary_implementation_core_feature_implementation.py",
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
    assert payload["mode"] == "m268-d002-live-continuation-runtime-integration-v1"
    assert payload["contract_id"] == "objc3c-part7-live-continuation-runtime-integration/m268-d002-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
