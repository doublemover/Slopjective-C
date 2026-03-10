from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m258_e001_cross_module_object_model_gate_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m258" / "M258-E001" / "cross_module_object_model_gate_summary.json"
CONTRACT_ID = "objc3c-cross-module-object-model-gate/m258-e001-v1"


def test_m258_e001_checker_emits_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(SCRIPT)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["contract_id"] == CONTRACT_ID
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["next_closeout_issue"] == "M258-E002"
    assert payload["upstream_summaries"]["M258-A002"]["contract_id"] == "objc3c-runtime-aware-import-module-frontend-closure/m258-a002-v1"
    assert payload["upstream_summaries"]["M258-B002"]["contract_id"] == "objc3c-imported-runtime-metadata-semantic-rules/m258-b002-v1"
    assert payload["upstream_summaries"]["M258-C002"]["contract_id"] == "objc3c-serialized-runtime-metadata-artifact-reuse/m258-c002-v1"
    assert payload["upstream_summaries"]["M258-D002"]["contract_id"] == "objc3c-cross-module-runtime-packaging-link-plan/m258-d002-v1"
