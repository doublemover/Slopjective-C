from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
APPLY = ROOT / "tmp" / "github-publish" / "cleanup_acceleration_program" / "apply_m317_b002_existing_amendments.py"
CHECKER = ROOT / "scripts" / "check_m317_b002_existing_milestone_and_issue_amendments_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m317" / "M317-B002" / "existing_milestone_issue_amendments_implementation_summary.json"


def test_m317_b002_checker_writes_static_summary() -> None:
    apply_completed = subprocess.run(
        [sys.executable, str(APPLY)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    assert apply_completed.returncode == 0, apply_completed.stderr or apply_completed.stdout
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
    assert payload["contract_id"] == "objc3c-cleanup-existing-milestone-issue-amendments-implementation/m317-b002-v1"
    assert payload["mode"] == "m317-b002-existing-milestone-issue-amendments-implementation-v1"
    assert payload["next_issue"] == "M317-B003"
    assert payload["target_milestone_numbers"] == [373]
    assert len(payload["target_issue_numbers"]) == 10
    assert payload["ok"] is True


def test_m317_b002_checker_writes_github_summary() -> None:
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
    assert "7529" in github_summary["issues"]
