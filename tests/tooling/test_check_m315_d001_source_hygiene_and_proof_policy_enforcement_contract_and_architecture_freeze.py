from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys


REPO_ROOT = Path(__file__).resolve().parents[2]
CHECKER = (
    REPO_ROOT
    / "scripts"
    / "check_m315_d001_source_hygiene_and_proof_policy_enforcement_contract_and_architecture_freeze.py"
)
SUMMARY_PATH = (
    REPO_ROOT
    / "tmp"
    / "reports"
    / "m315"
    / "M315-D001"
    / "source_hygiene_proof_policy_enforcement_contract_summary.json"
)


def test_m315_d001_checker_passes_and_writes_expected_summary() -> None:
    result = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert result.returncode == 0, result.stderr
    summary = json.loads(SUMMARY_PATH.read_text(encoding="utf-8"))
    assert summary["d002_zero_target_classes"] == {
        "transitional_source_model": 12,
        "legacy_m248_surface_identifier": 34,
    }
    assert summary["current_b005_zero_target_residuals"] == {
        "transitional_source_model": 0,
        "legacy_m248_surface_identifier": 0,
    }
    assert summary["tracked_compiler_python_sources"] == []
    assert summary["synthetic_fixture_root"] == "tests/tooling/fixtures/native/library_cli_parity"
    assert summary["ok"] is True
