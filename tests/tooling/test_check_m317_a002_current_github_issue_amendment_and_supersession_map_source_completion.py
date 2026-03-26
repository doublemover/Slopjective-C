from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m317_a002_current_github_issue_amendment_and_supersession_map_source_completion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m317" / "M317-A002" / "current_github_issue_amendment_and_supersession_map_summary.json"


def test_m317_a002_checker_writes_static_summary() -> None:
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
    assert payload["contract_id"] == "objc3c-cleanup-current-github-amendment-supersession-map/m317-a002-v1"
    assert payload["mode"] == "m317-a002-current-github-amendment-supersession-map-v1"
    assert payload["next_issue"] == "M317-B001"
    assert payload["amendment_issue_numbers"] == [7399, 7421, 7425, 7428, 7434, 7438, 7441]
    assert "M293" in payload["milestone_boundary_codes"]
    assert payload["ok"] is True


def test_m317_a002_checker_writes_github_summary() -> None:
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
