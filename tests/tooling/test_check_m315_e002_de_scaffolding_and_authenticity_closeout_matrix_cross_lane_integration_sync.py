from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys


REPO_ROOT = Path(__file__).resolve().parents[2]
CHECKER = (
    REPO_ROOT
    / "scripts"
    / "check_m315_e002_de_scaffolding_and_authenticity_closeout_matrix_cross_lane_integration_sync.py"
)
SUMMARY_PATH = (
    REPO_ROOT
    / "tmp"
    / "reports"
    / "m315"
    / "M315-E002"
    / "de_scaffolding_authenticity_closeout_matrix_summary.json"
)


def test_m315_e002_checker_passes_and_writes_expected_summary() -> None:
    result = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert result.returncode == 0, result.stderr
    summary = json.loads(SUMMARY_PATH.read_text(encoding="utf-8"))
    assert summary["final_b005_match_count"] == 57
    assert summary["final_remaining_residual_classes"] == {
        "dependency_issue_array": 3,
        "issue_key_schema_field": 8,
        "legacy_fixture_path_reference": 6,
        "next_issue_schema_field": 40,
    }
    assert summary["final_current_zero_target_residuals"] == {
        "transitional_source_model": 0,
        "legacy_m248_surface_identifier": 0,
    }
    assert summary["stable_identifier_family"] == "advanced_integration_closeout_signoff"
    assert summary["next_issue"] == "M318-A001"
