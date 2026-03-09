from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m263_e001_bootstrap_completion_conformance_gate_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m263" / "M263-E001" / "bootstrap_completion_conformance_gate_summary.json"


def test_checker_passes() -> None:
    result = subprocess.run(
        [sys.executable, str(SCRIPT)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    assert result.returncode == 0, result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["mode"] == "m263-e001-bootstrap-completion-conformance-gate-v1"
    assert payload["contract_id"] == "objc3c-runtime-bootstrap-completion-gate/m263-e001-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert payload["evidence_model"] == "a002-b003-c003-d003-summary-chain"
    assert payload["failure_model"] == "fail-closed-on-bootstrap-completion-evidence-drift"
    assert payload["next_closeout_issue"] == "M263-E002"

    upstream = payload["upstream_evidence"]
    assert upstream["M263-A002"]["ok"] is True
    assert upstream["M263-A002"]["explicit_backend"] == "llvm-direct"
    assert upstream["M263-A002"]["default_backend"] == "llvm-direct"
    assert upstream["M263-A002"]["artifact"] == "module.runtime-registration-descriptor.json"

    assert upstream["M263-B003"]["ok"] is True
    assert upstream["M263-B003"]["default_startup_registered_image_count"] == 1
    assert upstream["M263-B003"]["default_unsupported_replay_status"] == -1
    assert upstream["M263-B003"]["default_second_restart_replay_generation"] == 2

    assert upstream["M263-C003"]["ok"] is True
    assert upstream["M263-C003"]["object_format"] == "coff"
    assert upstream["M263-C003"]["plain_startup_registered_image_count"] == 0
    assert upstream["M263-C003"]["single_startup_registered_image_count"] == 1
    assert upstream["M263-C003"]["merged_startup_registered_image_count"] == 2
    assert upstream["M263-C003"]["merged_driver_linker_flag_count"] == 2

    assert upstream["M263-D003"]["ok"] is True
    assert upstream["M263-D003"]["default_startup_registered_image_count"] == 1
    assert upstream["M263-D003"]["default_post_reset_registered_image_count"] == 0
    assert upstream["M263-D003"]["default_second_restart_replay_generation"] == 2
