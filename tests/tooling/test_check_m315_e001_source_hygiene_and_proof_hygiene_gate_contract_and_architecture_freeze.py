from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys


REPO_ROOT = Path(__file__).resolve().parents[2]
CHECKER = (
    REPO_ROOT
    / "scripts"
    / "check_m315_e001_source_hygiene_and_proof_hygiene_gate_contract_and_architecture_freeze.py"
)
SUMMARY_PATH = (
    REPO_ROOT
    / "tmp"
    / "reports"
    / "m315"
    / "M315-E001"
    / "source_hygiene_proof_hygiene_gate_summary.json"
)


def test_m315_e001_checker_passes_and_writes_expected_summary() -> None:
    result = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert result.returncode == 0, result.stderr
    summary = json.loads(SUMMARY_PATH.read_text(encoding="utf-8"))
    assert summary["remaining_residual_classes"] == {
        "dependency_issue_array": 3,
        "issue_key_schema_field": 8,
        "legacy_fixture_path_reference": 6,
        "next_issue_schema_field": 40,
    }
    assert summary["current_zero_target_residuals"] == {
        "transitional_source_model": 0,
        "legacy_m248_surface_identifier": 0,
    }
    assert summary["stable_identifier_family"] == "advanced_integration_closeout_signoff"
