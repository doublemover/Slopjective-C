from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m254_e002_replay_and_bootstrap_proof_plus_runbook_closeout.py"
SUMMARY = ROOT / "tmp" / "reports" / "m254" / "M254-E002" / "replay_bootstrap_runbook_closeout_summary.json"


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
    assert payload["mode"] == "m254-e002-replay-bootstrap-runbook-closeout-v1"
    assert payload["contract_id"] == "objc3c-runtime-replay-bootstrap-closeout/m254-e002-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["closeout_model"] == "e001-gate-plus-live-operator-runbook-smoke"
    assert payload["runbook"] == "docs/runbooks/m254_bootstrap_replay_operator_runbook.md"

    upstream = payload["upstream_evidence"]
    assert upstream["M254-E001"]["ok"] is True
    assert upstream["M254-E001"]["contract_id"] == "objc3c-runtime-startup-registration-gate/m254-e001-v1"
    assert upstream["M254-D004"]["ok"] is True
    assert upstream["M254-D004"]["contract_id"] == "objc3c-runtime-launch-integration/m254-d004-v1"
    assert upstream["M254-D004"]["status"] == "PASS"

    probe = payload["runbook_probe"]
    assert probe["skipped"] is True
