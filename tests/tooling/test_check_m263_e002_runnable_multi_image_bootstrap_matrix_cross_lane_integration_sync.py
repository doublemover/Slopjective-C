from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m263_e002_runnable_multi_image_bootstrap_matrix_cross_lane_integration_sync.py"
SUMMARY = ROOT / "tmp" / "reports" / "m263" / "M263-E002" / "bootstrap_matrix_closeout_summary.json"


def test_checker_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    assert result.returncode == 0, result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["mode"] == "m263-e002-runnable-multi-image-bootstrap-matrix-closeout-v1"
    assert payload["contract_id"] == "objc3c-runtime-runnable-bootstrap-matrix-closeout/m263-e002-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["closeout_model"] == "e001-gate-plus-published-bootstrap-matrix-runbook"
    assert payload["runbook"] == "docs/runbooks/m263_bootstrap_matrix_operator_runbook.md"

    upstream = payload["upstream_evidence"]
    assert upstream["M263-E001"]["ok"] is True
    assert upstream["M263-E001"]["contract_id"] == "objc3c-runtime-bootstrap-completion-gate/m263-e001-v1"
    assert upstream["M263-C003"]["ok"] is True
    assert upstream["M263-C003"]["contract_id"] == "objc3c-runtime-bootstrap-archive-static-link-replay-corpus/m263-c003-v1"
    assert upstream["M263-D003"]["ok"] is True
    assert upstream["M263-D003"]["contract_id"] == "objc3c-runtime-live-restart-hardening/m263-d003-v1"

    probe = payload["matrix_probe"]
    assert probe["skipped"] is True
