from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m317_a001_backlog_overlap_and_correction_inventory_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m317" / "M317-A001" / "backlog_overlap_correction_inventory_summary.json"


def test_m317_a001_checker_writes_static_summary() -> None:
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
    assert payload["contract_id"] == "objc3c-cleanup-backlog-overlap-correction-inventory/m317-a001-v1"
    assert payload["mode"] == "m317-a001-backlog-overlap-correction-inventory-v1"
    assert payload["next_issue"] == "M317-A002"
    assert payload["inventory_family_ids"] == [
        "future_post_m292_dependency_consumers",
        "human_facing_superclean_boundary",
        "realized_dispatch_corrective_overlap",
        "scaffold_retirement_scope_narrowing",
        "synthesized_accessor_corrective_overlap",
    ]
    assert payload["ok"] is True


def test_m317_a001_checker_writes_github_summary() -> None:
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
