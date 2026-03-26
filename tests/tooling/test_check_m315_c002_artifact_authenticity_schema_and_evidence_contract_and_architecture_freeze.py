from __future__ import annotations

import json
from pathlib import Path
import subprocess
import sys


REPO_ROOT = Path(__file__).resolve().parents[2]
CHECKER = (
    REPO_ROOT
    / "scripts"
    / "check_m315_c002_artifact_authenticity_schema_and_evidence_contract_and_architecture_freeze.py"
)
SUMMARY_PATH = (
    REPO_ROOT
    / "tmp"
    / "reports"
    / "m315"
    / "M315-C002"
    / "artifact_authenticity_schema_summary.json"
)


def test_m315_c002_checker_passes_and_writes_expected_summary() -> None:
    result = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert result.returncode == 0, result.stderr
    summary = json.loads(SUMMARY_PATH.read_text(encoding="utf-8"))
    assert summary["stable_class_count"] == 7
    assert summary["provenance_mode_count"] == 8
    assert summary["proof_eligible_classes"] == ["generated_replay"]
    assert summary["a003_replay_candidate_missing_provenance"] == 152
    assert summary["b004_replay_without_frontend_header"] == 46
    assert summary["c001_authenticity_schema_owner_issue"] == "M315-C002"
