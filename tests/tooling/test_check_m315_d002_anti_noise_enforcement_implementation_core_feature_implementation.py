from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys


REPO_ROOT = Path(__file__).resolve().parents[2]
CHECKER = (
    REPO_ROOT
    / "scripts"
    / "check_m315_d002_anti_noise_enforcement_implementation_core_feature_implementation.py"
)
SUMMARY_PATH = (
    REPO_ROOT
    / "tmp"
    / "reports"
    / "m315"
    / "M315-D002"
    / "anti_noise_enforcement_summary.json"
)


def test_m315_d002_checker_passes_and_writes_expected_summary() -> None:
    result = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert result.returncode == 0, result.stderr
    summary = json.loads(SUMMARY_PATH.read_text(encoding="utf-8"))
    assert summary["post_cleanup_native_source_milestone_token_lines"] == 57
    assert summary["remaining_quarantined_residual_classes"] == {
        "dependency_issue_array": 3,
        "issue_key_schema_field": 8,
        "legacy_fixture_path_reference": 6,
        "next_issue_schema_field": 40,
    }
    assert summary["stable_identifier_family"] == "advanced_integration_closeout_signoff"
    assert summary["tracked_compiler_python_sources"] == []
    assert all(not hits for hits in summary["target_file_zero_target_hits"].values())
