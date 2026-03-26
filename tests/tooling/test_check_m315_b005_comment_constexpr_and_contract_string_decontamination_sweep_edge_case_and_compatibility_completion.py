from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys


REPO_ROOT = Path(__file__).resolve().parents[2]
CHECKER = (
    REPO_ROOT
    / "scripts"
    / "check_m315_b005_comment_constexpr_and_contract_string_decontamination_sweep_edge_case_and_compatibility_completion.py"
)
SUMMARY_PATH = (
    REPO_ROOT
    / "tmp"
    / "reports"
    / "m315"
    / "M315-B005"
    / "comment_constexpr_contract_string_decontamination_summary.json"
)


def test_m315_b005_checker_passes_and_writes_expected_summary() -> None:
    result = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert result.returncode == 0, result.stderr
    summary = json.loads(SUMMARY_PATH.read_text(encoding="utf-8"))
    assert summary["match_count"] == 103
    assert summary["disallowed_count"] == 0
    assert summary["residual_class_counts"] == {
        "dependency_issue_array": 3,
        "issue_key_schema_field": 8,
        "legacy_fixture_path_reference": 6,
        "legacy_m248_surface_identifier": 34,
        "next_issue_schema_field": 40,
        "transitional_source_model": 12,
    }
