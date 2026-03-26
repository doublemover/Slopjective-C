from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m318_b003_maintainer_review_checklist_and_regression_handling_model_edge_case_and_compatibility_completion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m318" / "M318-B003" / "review_regression_model_summary.json"


def test_m318_b003_checker_passes_and_writes_summary() -> None:
    result = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert result.returncode == 0, result.stderr
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-governance-review-and-regression-model/m318-b003-v1"
    assert payload["required_review_questions"] == [
        "budget_family_changed",
        "exception_record_if_required",
        "validation_posture_and_evidence",
        "rollback_or_regression_path",
    ]
    assert payload["package_governance"]["reviewChecklistOwnerIssue"] == "M318-B003"
