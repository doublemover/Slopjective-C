from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion.py"
TEST_SUMMARY = ROOT / "tmp" / "reports" / "m261" / "M261-B003" / "byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_summary.test.json"
CONTRACT_ID = "objc3c-executable-block-byref-copy-dispose-and-object-capture-ownership/m261-b003-v1"


def test_m261_b003_checker_emits_summary() -> None:
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
    assert payload["owned_positive_case"]["skipped"] is True
    assert payload["next_issue"] == "M261-C001"
