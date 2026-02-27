from __future__ import annotations

import importlib.util
import io
import json
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

import pytest

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "check_bootstrap_readiness.py"
SPEC = importlib.util.spec_from_file_location("check_bootstrap_readiness", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_bootstrap_readiness.py for tests.")
check_bootstrap_readiness = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = check_bootstrap_readiness
SPEC.loader.exec_module(check_bootstrap_readiness)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "bootstrap_readiness"


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = check_bootstrap_readiness.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def run_scenario(name: str, *, output_format: str) -> tuple[int, str, str]:
    scenario_root = FIXTURE_ROOT / name
    args = [
        "--issues-json",
        str(scenario_root / "issues.json"),
        "--milestones-json",
        str(scenario_root / "milestones.json"),
        "--catalog-json",
        str(scenario_root / "catalog.json"),
        "--format",
        output_format,
    ]
    open_blockers_path = scenario_root / "open_blockers.json"
    if open_blockers_path.exists():
        args.extend(["--open-blockers-json", str(open_blockers_path)])
    return run_main(args)


def read_fixture_text(
    scenario_name: str,
    file_name: str,
    *,
    ensure_trailing_newline: bool = False,
) -> str:
    text = (FIXTURE_ROOT / scenario_name / file_name).read_text(encoding="utf-8")
    if ensure_trailing_newline and not text.endswith("\n"):
        return f"{text}\n"
    return text


def test_json_all_zero_counts_is_bootstrappable() -> None:
    code, stdout, stderr = run_scenario("all_zero", output_format="json")

    assert code == 0
    assert stderr == ""

    payload = json.loads(stdout)
    assert payload["issues_open_count"] == 0
    assert payload["milestones_open_count"] == 0
    assert payload["catalog_open_task_count"] == 0
    assert payload["blockers_open_count"] == 0
    assert payload["readiness_state"] == "bootstrappable"
    assert payload["blocking_dimensions"] == []


def test_json_open_issues_only_returns_exit_1_and_lists_issues_dimension() -> None:
    code, stdout, stderr = run_scenario("open_issues_only", output_format="json")

    assert code == 1
    assert stderr == ""

    payload = json.loads(stdout)
    assert payload["readiness_state"] == "blocked"
    assert payload["blocking_dimensions"] == ["issues_open_count"]


def test_json_open_milestones_only_returns_exit_1() -> None:
    code, stdout, stderr = run_scenario("open_milestones_only", output_format="json")

    assert code == 1
    assert stderr == ""

    payload = json.loads(stdout)
    assert payload["readiness_state"] == "blocked"
    assert payload["blocking_dimensions"] == ["milestones_open_count"]


def test_json_open_catalog_tasks_only_returns_exit_1() -> None:
    code, stdout, stderr = run_scenario("open_catalog_tasks_only", output_format="json")

    assert code == 1
    assert stderr == ""

    payload = json.loads(stdout)
    assert payload["readiness_state"] == "blocked"
    assert payload["blocking_dimensions"] == ["catalog_open_task_count"]


def test_json_open_blockers_only_returns_exit_1() -> None:
    code, stdout, stderr = run_scenario("open_blockers_only", output_format="json")

    assert code == 1
    assert stderr == ""

    payload = json.loads(stdout)
    assert payload["readiness_state"] == "blocked"
    assert payload["blocking_dimensions"] == ["blockers_open_count"]


def test_json_canonical_blocker_snapshot_matches_expected_output() -> None:
    expected = read_fixture_text(
        "canonical_blockers_snapshot",
        "expected.json",
        ensure_trailing_newline=True,
    )

    code, stdout, stderr = run_scenario("canonical_blockers_snapshot", output_format="json")

    assert code == 1
    assert stderr == ""
    assert stdout == expected


def test_markdown_canonical_blocker_snapshot_matches_expected_output() -> None:
    expected = read_fixture_text(
        "canonical_blockers_snapshot",
        "expected.md",
        ensure_trailing_newline=True,
    )

    code, stdout, stderr = run_scenario("canonical_blockers_snapshot", output_format="md")

    assert code == 1
    assert stderr == ""
    assert stdout == expected


def test_json_legacy_blocker_array_payload_is_accepted() -> None:
    expected = read_fixture_text(
        "legacy_blockers_array_payload",
        "expected.json",
        ensure_trailing_newline=True,
    )

    code, stdout, stderr = run_scenario("legacy_blockers_array_payload", output_format="json")

    assert code == 1
    assert stderr == ""
    assert stdout == expected


@pytest.mark.parametrize(
    "scenario_name",
    (
        "hard_fail_open_blockers_metadata_pair_mismatch",
        "hard_fail_open_blockers_count_mismatch",
        "hard_fail_open_blockers_unsorted_rows",
        "hard_fail_open_blockers_duplicate_rows",
        "hard_fail_open_blockers_line_alias_mismatch",
    ),
)
def test_fixture_backed_open_blocker_fail_closed_paths(scenario_name: str) -> None:
    scenario_root = FIXTURE_ROOT / scenario_name
    expected_exit_code = int((scenario_root / "expected_exit_code.txt").read_text(encoding="utf-8").strip())
    expected_stderr = (scenario_root / "expected_stderr.txt").read_text(encoding="utf-8")

    code, stdout, stderr = run_scenario(scenario_name, output_format="json")

    assert code == expected_exit_code
    assert stdout == ""
    assert stderr == expected_stderr


def test_json_malformed_input_returns_exit_2() -> None:
    code, stdout, stderr = run_scenario("malformed_issues_json", output_format="json")

    assert code == 2
    assert stdout == ""
    assert "invalid json" in stderr.lower()
    assert "issues.json" in stderr


def test_markdown_all_zero_includes_header_and_key_lines() -> None:
    code, stdout, stderr = run_scenario("all_zero", output_format="md")

    assert code == 0
    assert stderr == ""
    assert stdout.startswith("# Bootstrap Readiness\n")
    assert "| readiness_state | `bootstrappable` |" in stdout
    assert "| issues_open_count | `0` |" in stdout
    assert "| milestones_open_count | `0` |" in stdout


def test_markdown_blocked_path_includes_readiness_and_metric_rows() -> None:
    code, stdout, stderr = run_scenario("open_issues_only", output_format="md")

    assert code == 1
    assert stderr == ""
    assert stdout.startswith("# Bootstrap Readiness\n")
    assert "| readiness_state | `blocked` |" in stdout
    assert "| issues_open_count | `1` |" in stdout
    assert "| intake_recommendation | `hold` |" in stdout
