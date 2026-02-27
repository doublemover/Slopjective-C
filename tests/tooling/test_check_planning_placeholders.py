from __future__ import annotations

import importlib.util
import io
import json
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "check_planning_placeholders.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_planning_placeholders",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_planning_placeholders.py")

check_planning_placeholders = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = check_planning_placeholders
SPEC.loader.exec_module(check_planning_placeholders)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "planning_placeholders"
SCAN_ROOT = FIXTURE_ROOT / "spec" / "planning"
ALLOWLIST_PARTIAL = FIXTURE_ROOT / "allowlist_partial.json"
ALLOWLIST_FULL = FIXTURE_ROOT / "allowlist_full.json"
ALLOWLIST_UNUSED = FIXTURE_ROOT / "allowlist_unused.json"
ALLOWLIST_INVALID = FIXTURE_ROOT / "allowlist_invalid.json"
ALLOWLIST_INVALID_SCHEMA = FIXTURE_ROOT / "allowlist_invalid_schema.json"
ALLOWLIST_INVALID_SHAPE = FIXTURE_ROOT / "allowlist_invalid_shape.json"


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = check_planning_placeholders.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def test_json_output_reports_unresolved_placeholders() -> None:
    code, output, stderr = run_main(
        [
            "--root",
            str(SCAN_ROOT),
            "--allowlist",
            str(ALLOWLIST_PARTIAL),
            "--format",
            "json",
        ]
    )

    assert code == 1
    assert stderr == ""

    payload = json.loads(output)
    assert payload["scanned_file_count"] == 2
    assert payload["placeholder_count"] == 7
    assert payload["allowlisted_count"] == 2
    assert payload["unresolved_count"] == 5
    assert payload["unused_allowlist_entry_count"] == 0
    assert payload["unused_allowlist_entries"] == []

    unresolved = payload["unresolved"]
    assert isinstance(unresolved, list)
    assert len(unresolved) == 5
    assert [entry["token"] for entry in unresolved] == [
        "<sha-*>",
        "<output>",
        "<yes/no + note>",
        "<DATE>",
        "<YYYY-MM-DD>",
    ]
    assert unresolved[0]["match"] == "<sha-lane-a>"
    assert unresolved[0]["path"].endswith("tests/tooling/fixtures/planning_placeholders/spec/planning/a_root.md")


def test_human_output_is_ok_when_everything_is_allowlisted() -> None:
    code, output, stderr = run_main(
        [
            "--root",
            str(SCAN_ROOT),
            "--allowlist",
            str(ALLOWLIST_FULL),
            "--format",
            "human",
        ]
    )

    assert code == 0
    assert stderr == ""
    assert "planning-placeholder-lint: OK" in output
    assert "scanned_files=2" in output
    assert "placeholders=7" in output
    assert "allowlisted=7" in output
    assert "unused_allowlist_entries=0" in output


def test_unused_allowlist_entries_fail_by_default() -> None:
    code, output, stderr = run_main(
        [
            "--root",
            str(SCAN_ROOT),
            "--allowlist",
            str(ALLOWLIST_UNUSED),
            "--format",
            "json",
        ]
    )

    assert code == 1
    assert stderr == ""

    payload = json.loads(output)
    assert payload["unresolved_count"] == 0
    assert payload["unused_allowlist_entry_count"] == 1
    assert payload["unused_allowlist_entries"] == [
        {
            "path": "tests/tooling/fixtures/planning_placeholders/spec/planning/a_root.md",
            "token": "<DATE>",
            "reason": "Intentionally stale entry for migration test coverage.",
        }
    ]


def test_unused_allowlist_entries_can_be_ignored_with_migration_flag() -> None:
    code, output, stderr = run_main(
        [
            "--root",
            str(SCAN_ROOT),
            "--allowlist",
            str(ALLOWLIST_UNUSED),
            "--format",
            "json",
            "--allow-unused-allowlist-entries",
        ]
    )

    assert code == 0
    assert stderr == ""

    payload = json.loads(output)
    assert payload["unresolved_count"] == 0
    assert payload["unused_allowlist_entry_count"] == 1
    assert payload["unused_allowlist_entries"] == [
        {
            "path": "tests/tooling/fixtures/planning_placeholders/spec/planning/a_root.md",
            "token": "<DATE>",
            "reason": "Intentionally stale entry for migration test coverage.",
        }
    ]


def test_invalid_allowlist_fails_with_exit_code_2() -> None:
    code, output, stderr = run_main(
        [
            "--root",
            str(SCAN_ROOT),
            "--allowlist",
            str(ALLOWLIST_INVALID),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert output == ""
    assert "allowlist entry 0 field 'reason' must be a non-empty string" in stderr


def test_invalid_allowlist_schema_version_fails_with_exit_code_2() -> None:
    code, output, stderr = run_main(
        [
            "--root",
            str(SCAN_ROOT),
            "--allowlist",
            str(ALLOWLIST_INVALID_SCHEMA),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert output == ""
    assert "allowlist JSON field 'version' must be the integer 1" in stderr


def test_invalid_allowlist_entry_shape_fails_with_exit_code_2() -> None:
    code, output, stderr = run_main(
        [
            "--root",
            str(SCAN_ROOT),
            "--allowlist",
            str(ALLOWLIST_INVALID_SHAPE),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert output == ""
    assert "allowlist entry 0 has unexpected field(s): owner" in stderr
