from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m317_b001_immediate_backlog_surgery_and_supersession_model_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m317" / "M317-B001" / "immediate_backlog_surgery_and_supersession_model_summary.json"


def test_m317_b001_checker_writes_static_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-github-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["contract_id"] == "objc3c-cleanup-immediate-backlog-surgery-supersession-model/m317-b001-v1"
    assert payload["mode"] == "m317-b001-immediate-backlog-surgery-supersession-model-v1"
    assert payload["next_issue"] == "M317-B002"
    assert payload["allowed_action_names"] == [
        "boundary_preserve",
        "cleanup_first_block",
        "concept_hold",
        "corrective_tranche_consumes_existing_scope",
        "future_dependency_rewrite",
        "narrow_in_place",
    ]
    assert payload["ok"] is True


def test_m317_b001_checker_writes_github_summary() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert completed.returncode == 0, completed.stderr or completed.stdout
    payload = json.loads(SUMMARY.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    github_summary = payload["github_summary"]
    assert github_summary["unlabeled_open_issues"] == []
    assert github_summary["open_issues_missing_execution_order"] == []
    assert "7399" in github_summary["issues"]
