from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m261_a002_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation.py"
TEST_SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-A002" / "block_source_model_completion_summary.test.json"
CONTRACT_ID = "objc3c-executable-block-source-model-completion/m261-a002-v1"


def test_m261_a002_checker_emits_summary() -> None:
    completed = subprocess.run(
        [
            sys.executable,
            str(SCRIPT),
            "--skip-dynamic-probes",
            "--summary-out",
            str(TEST_SUMMARY),
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(TEST_SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["dynamic_probes_executed"] is False
    assert payload["dynamic_case"]["skipped"] is True
    assert payload["next_issue"] == "M261-B001"
