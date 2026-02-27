from __future__ import annotations

import importlib.util
import io
import json
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

import pytest

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "check_planning_unchecked_checkboxes.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_planning_unchecked_checkboxes",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_planning_unchecked_checkboxes.py"
    )

check_planning_unchecked_checkboxes = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = check_planning_unchecked_checkboxes
SPEC.loader.exec_module(check_planning_unchecked_checkboxes)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "planning_checkboxes"


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = check_planning_unchecked_checkboxes.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def fixture_root(name: str) -> Path:
    return FIXTURE_ROOT / name


def test_disallowed_unchecked_row_fails_human_output() -> None:
    root = fixture_root("disallowed")
    code, stdout, stderr = run_main(["--root", str(root)])

    assert code == 1
    assert stderr == ""
    assert "planning-unchecked-checkboxes: FAIL" in stdout
    assert "disallowed_count=1" in stdout
    assert "spec/planning/plan.md:5" in stdout


def test_policy_exception_allows_unchecked_row_and_emits_reason() -> None:
    root = fixture_root("allowed_section")
    code, stdout, stderr = run_main(["--root", str(root), "--format", "json"])

    assert code == 0
    assert stderr == ""

    payload = json.loads(stdout)
    assert payload["summary"]["unchecked_row_count"] == 1
    assert payload["summary"]["allowed_count"] == 1
    assert payload["summary"]["disallowed_count"] == 0
    assert payload["summary"]["unused_policy_entry_count"] == 0
    assert payload["allowed"][0]["path"] == "spec/planning/plan.md"
    assert (
        payload["allowed"][0]["section"]
        == "Runbook Fixture > 1. Future Rerun Checklist"
    )
    assert "reason" in payload["allowed"][0]
    assert "Procedural runbook section" in payload["allowed"][0]["reason"]
    assert payload["unused_policy_entries"] == []


def test_unused_policy_entry_fails_by_default() -> None:
    root = fixture_root("unused_policy_entry")
    code, stdout, stderr = run_main(["--root", str(root), "--format", "json"])

    assert code == 1
    assert stderr == ""

    payload = json.loads(stdout)
    assert payload["summary"] == {
        "scanned_file_count": 1,
        "unchecked_row_count": 0,
        "allowed_count": 0,
        "disallowed_count": 0,
        "unused_policy_entry_count": 1,
    }
    assert payload["allowed"] == []
    assert payload["disallowed"] == []
    assert payload["unused_policy_entries"] == [
        {
            "path": "spec/planning/plan.md",
            "section": "Unused Fixture > 1. Historical Checklist",
            "reason": "Intentional stale policy entry to verify strict detection.",
        }
    ]


def test_unused_policy_entry_bypass_flag_allows_migration() -> None:
    root = fixture_root("unused_policy_entry")
    code, stdout, stderr = run_main(
        [
            "--root",
            str(root),
            "--format",
            "json",
            "--allow-unused-policy-entries",
        ]
    )

    assert code == 0
    assert stderr == ""

    payload = json.loads(stdout)
    assert payload["summary"]["unused_policy_entry_count"] == 1
    assert payload["unused_policy_entries"][0]["path"] == "spec/planning/plan.md"
    assert (
        payload["unused_policy_entries"][0]["section"]
        == "Unused Fixture > 1. Historical Checklist"
    )


def test_policy_path_match_without_section_match_still_fails() -> None:
    root = fixture_root("section_mismatch")
    code, stdout, stderr = run_main(["--root", str(root), "--format", "json"])

    assert code == 1
    assert stderr == ""

    payload = json.loads(stdout)
    assert payload["summary"]["unchecked_row_count"] == 1
    assert payload["summary"]["allowed_count"] == 0
    assert payload["summary"]["disallowed_count"] == 1
    assert payload["summary"]["unused_policy_entry_count"] == 1
    assert payload["disallowed"][0]["path"] == "spec/planning/plan.md"
    assert (
        payload["disallowed"][0]["section"]
        == "Runbook Fixture > 1. Future Rerun Checklist"
    )
    assert payload["unused_policy_entries"] == [
        {
            "path": "spec/planning/plan.md",
            "section": "Runbook Fixture > 1. Historical Checklist",
            "reason": "Intentional mismatch to verify strict section matching.",
        }
    ]


def test_json_output_is_deterministic_for_mixed_fixture() -> None:
    root = fixture_root("json_mixed")
    code, stdout, stderr = run_main(["--root", str(root), "--format", "json"])

    assert code == 1
    assert stderr == ""

    payload = json.loads(stdout)
    assert payload["summary"] == {
        "scanned_file_count": 2,
        "unchecked_row_count": 2,
        "allowed_count": 1,
        "disallowed_count": 1,
        "unused_policy_entry_count": 0,
    }
    assert [entry["path"] for entry in payload["allowed"]] == ["spec/planning/a.md"]
    assert [entry["path"] for entry in payload["disallowed"]] == ["spec/planning/b.md"]
    assert "reason" in payload["allowed"][0]
    assert "reason" not in payload["disallowed"][0]
    assert payload["unused_policy_entries"] == []


def test_invalid_policy_shape_returns_exit_2(tmp_path: Path) -> None:
    planning_dir = tmp_path / "spec" / "planning"
    planning_dir.mkdir(parents=True)

    (planning_dir / "plan.md").write_text(
        "# Invalid Policy Fixture\n\n## 1. Tasks\n\n- [ ] This row is present.\n",
        encoding="utf-8",
    )
    (planning_dir / "planning_checkbox_policy.json").write_text(
        json.dumps({"allowed_unchecked_sections": []}, indent=2) + "\n",
        encoding="utf-8",
    )

    code, stdout, stderr = run_main(["--root", str(tmp_path), "--format", "json"])

    assert code == 2
    assert stdout == ""
    assert "policy field 'version' must be a positive integer" in stderr
