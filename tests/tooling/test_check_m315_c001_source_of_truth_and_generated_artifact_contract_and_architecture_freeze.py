from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys


REPO_ROOT = Path(__file__).resolve().parents[2]
CHECKER = (
    REPO_ROOT
    / "scripts"
    / "check_m315_c001_source_of_truth_and_generated_artifact_contract_and_architecture_freeze.py"
)
SUMMARY_PATH = (
    REPO_ROOT
    / "tmp"
    / "reports"
    / "m315"
    / "M315-C001"
    / "source_of_truth_generated_artifact_contract_summary.json"
)


def test_m315_c001_checker_passes_and_writes_expected_summary() -> None:
    result = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert result.returncode == 0, result.stderr
    summary = json.loads(SUMMARY_PATH.read_text(encoding="utf-8"))
    assert summary["observed_next_issue_ref_count"] == 37
    assert summary["observed_issue_key_field_count"] == 8
    assert summary["observed_dependency_ref_count"] == 11
    assert summary["observed_m248_identifier_count"] == 0
    assert summary["observed_transitional_literal_count"] == 0
    assert summary["residual_class_expectations"] == {
        "dependency_issue_array": 3,
        "issue_key_schema_field": 8,
        "next_issue_schema_field": 40,
    }
