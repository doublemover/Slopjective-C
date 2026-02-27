from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "run_bootstrap_readiness.py"
SPEC = importlib.util.spec_from_file_location("run_bootstrap_readiness", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/run_bootstrap_readiness.py for tests.")
run_bootstrap_readiness = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = run_bootstrap_readiness
SPEC.loader.exec_module(run_bootstrap_readiness)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "bootstrap_readiness_runner"


def run_scenario(tmp_path: Path, name: str) -> tuple[int, Path]:
    scenario_root = FIXTURE_ROOT / name
    output_dir = tmp_path / f"{name}_artifacts"
    args = [
        "--issues-json",
        str(scenario_root / "issues.json"),
        "--milestones-json",
        str(scenario_root / "milestones.json"),
        "--catalog-json",
        str(scenario_root / "catalog.json"),
        "--open-blockers-json",
        str(scenario_root / "open_blockers.json"),
        "--output-dir",
        str(output_dir),
    ]
    code = run_bootstrap_readiness.main(args)
    return code, output_dir


def read_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def render_checker_markdown(payload: dict[str, object]) -> str:
    lines = [
        "# Bootstrap Readiness",
        "",
        "| Metric | Value |",
        "| --- | --- |",
        f"| issues_open_count | `{payload['issues_open_count']}` |",
        f"| milestones_open_count | `{payload['milestones_open_count']}` |",
        f"| catalog_open_task_count | `{payload['catalog_open_task_count']}` |",
        f"| blockers_open_count | `{payload['blockers_open_count']}` |",
        f"| readiness_state | `{payload['readiness_state']}` |",
        f"| intake_recommendation | `{payload['intake_recommendation']}` |",
    ]
    return "\n".join(lines) + "\n"


def test_runner_success_path_persists_artifacts_and_returns_zero(tmp_path: Path) -> None:
    code, output_dir = run_scenario(tmp_path, "zero_open")

    assert code == 0

    checker_json_path = output_dir / "check_bootstrap_readiness.json"
    checker_md_path = output_dir / "check_bootstrap_readiness.md"
    checker_json_log_path = output_dir / "check_bootstrap_readiness_json.log"
    checker_md_log_path = output_dir / "check_bootstrap_readiness_markdown.log"
    summary_path = output_dir / "bootstrap_readiness_summary.json"
    report_path = output_dir / "bootstrap_readiness_report.md"

    for artifact in (
        checker_json_path,
        checker_md_path,
        checker_json_log_path,
        checker_md_log_path,
        summary_path,
        report_path,
    ):
        assert artifact.exists()
        assert artifact.is_file()
        assert b"\r" not in artifact.read_bytes()

    summary = read_json(summary_path)
    assert list(summary.keys()) == [
        "runner",
        "inputs",
        "artifacts",
        "open_blockers_refresh",
        "readiness",
        "spec_lint",
        "commands",
        "errors",
        "final_status",
        "final_exit_code",
    ]
    assert summary["final_status"] == "ok"
    assert summary["final_exit_code"] == 0

    readiness = summary["readiness"]
    assert isinstance(readiness, dict)
    assert readiness["readiness_state"] == "bootstrappable"
    assert readiness["intake_recommendation"] == "go"
    assert readiness["issues_open_count"] == 0
    assert readiness["milestones_open_count"] == 0
    assert readiness["catalog_open_task_count"] == 0
    assert readiness["blockers_open_count"] == 0
    assert readiness["blocking_dimensions"] == []
    assert readiness["checker_exit_code"] == 0

    refresh = summary["open_blockers_refresh"]
    assert isinstance(refresh, dict)
    assert refresh["requested"] is False
    assert refresh["attempted"] is False
    assert refresh["exit_code"] is None
    assert refresh["root"] is None
    assert refresh["generated_at_utc"] is None
    assert refresh["source"] is None

    spec_lint = summary["spec_lint"]
    assert isinstance(spec_lint, dict)
    assert spec_lint["requested"] is False
    assert spec_lint["attempted"] is False
    assert spec_lint["exit_code"] is None
    assert spec_lint["ok"] is None

    artifacts = summary["artifacts"]
    assert isinstance(artifacts, dict)
    assert artifacts["open_blockers_refresh_log"] is None
    assert artifacts["spec_lint_log"] is None

    commands = summary["commands"]
    assert isinstance(commands, dict)
    assert list(commands.keys()) == [
        "check_bootstrap_readiness_json",
        "check_bootstrap_readiness_markdown",
    ]
    assert commands["check_bootstrap_readiness_json"]["exit_code"] == 0
    assert commands["check_bootstrap_readiness_markdown"]["exit_code"] == 0

    report_text = report_path.read_text(encoding="utf-8")
    assert "- Final status: `ok`" in report_text
    assert "- Final exit code: `0`" in report_text
    assert "- Open blockers refresh requested: `false`" in report_text


def test_runner_blocked_readiness_returns_one(tmp_path: Path) -> None:
    code, output_dir = run_scenario(tmp_path, "blocked_issues")

    assert code == 1

    summary = read_json(output_dir / "bootstrap_readiness_summary.json")
    assert summary["final_status"] == "blocked-readiness"
    assert summary["final_exit_code"] == 1

    readiness = summary["readiness"]
    assert isinstance(readiness, dict)
    assert readiness["readiness_state"] == "blocked"
    assert readiness["intake_recommendation"] == "hold"
    assert readiness["issues_open_count"] == 1
    assert readiness["blocking_dimensions"] == ["issues_open_count"]
    assert readiness["checker_exit_code"] == 1

    commands = summary["commands"]
    assert isinstance(commands, dict)
    assert commands["check_bootstrap_readiness_json"]["exit_code"] == 1
    assert commands["check_bootstrap_readiness_markdown"]["exit_code"] == 1

    report_text = (output_dir / "bootstrap_readiness_report.md").read_text(encoding="utf-8")
    assert "- Readiness state: `blocked`" in report_text
    assert "- Final exit code: `1`" in report_text


def test_runner_fails_closed_when_open_blockers_refresh_fails(
    tmp_path: Path,
    monkeypatch,
) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    output_dir = tmp_path / "refresh_fail_artifacts"
    open_blockers_path = output_dir / "inputs" / "open_blockers.snapshot.json"

    issues_path.write_text("[]\n", encoding="utf-8")
    milestones_path.write_text("[]\n", encoding="utf-8")
    catalog_path.write_text('{"tasks": []}\n', encoding="utf-8")

    checker_payload = {
        "issues_open_count": 0,
        "milestones_open_count": 0,
        "catalog_open_task_count": 0,
        "blockers_open_count": 0,
        "readiness_state": "bootstrappable",
        "intake_recommendation": "go",
        "blocking_dimensions": [],
    }
    checker_json = json.dumps(checker_payload, indent=2) + "\n"
    checker_md = render_checker_markdown(checker_payload)

    command_specs: list[run_bootstrap_readiness.CommandSpec] = []

    def fake_run_command(spec: run_bootstrap_readiness.CommandSpec) -> run_bootstrap_readiness.CommandResult:
        command_specs.append(spec)
        if spec.name == "extract_open_blockers_snapshot_json":
            return run_bootstrap_readiness.CommandResult(
                spec=spec,
                exit_code=2,
                stdout="",
                stderr="error: malformed OPEN blocker row\n",
            )
        if spec.name == "check_bootstrap_readiness_json":
            return run_bootstrap_readiness.CommandResult(
                spec=spec,
                exit_code=0,
                stdout=checker_json,
                stderr="",
            )
        if spec.name == "check_bootstrap_readiness_markdown":
            return run_bootstrap_readiness.CommandResult(
                spec=spec,
                exit_code=0,
                stdout=checker_md,
                stderr="",
            )
        raise AssertionError(f"unexpected command spec: {spec.name}")

    monkeypatch.setattr(run_bootstrap_readiness, "run_command", fake_run_command)

    code = run_bootstrap_readiness.main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--refresh-open-blockers",
            "--open-blockers-generated-at-utc",
            "2026-02-24T00:00:00Z",
            "--open-blockers-source",
            "fixture:bootstrap-readiness-refresh-fail",
            "--output-dir",
            str(output_dir),
        ]
    )

    assert code == 2
    assert [spec.name for spec in command_specs] == [
        "extract_open_blockers_snapshot_json",
        "check_bootstrap_readiness_json",
        "check_bootstrap_readiness_markdown",
    ]

    refresh_spec = command_specs[0]
    assert refresh_spec.actual_args == (
        "--root",
        str(run_bootstrap_readiness.DEFAULT_OPEN_BLOCKERS_ROOT),
        "--format",
        "snapshot-json",
        "--generated-at-utc",
        "2026-02-24T00:00:00Z",
        "--source",
        "fixture:bootstrap-readiness-refresh-fail",
    )

    assert not open_blockers_path.exists()
    assert (output_dir / "extract_open_blockers.log").exists()

    summary = read_json(output_dir / "bootstrap_readiness_summary.json")
    assert summary["final_status"] == "runner-error"
    assert summary["final_exit_code"] == 2

    inputs = summary["inputs"]
    assert isinstance(inputs, dict)
    assert inputs["open_blockers_refresh"] is True
    assert (
        inputs["open_blockers_root"]
        == run_bootstrap_readiness.display_path(run_bootstrap_readiness.DEFAULT_OPEN_BLOCKERS_ROOT)
    )
    assert inputs["open_blockers_generated_at_utc"] == "2026-02-24T00:00:00Z"
    assert inputs["open_blockers_source"] == "fixture:bootstrap-readiness-refresh-fail"
    assert (
        inputs["open_blockers_json"]
        == run_bootstrap_readiness.display_path(open_blockers_path)
    )

    refresh = summary["open_blockers_refresh"]
    assert isinstance(refresh, dict)
    assert refresh["requested"] is True
    assert refresh["attempted"] is True
    assert refresh["exit_code"] == 2
    assert (
        refresh["root"]
        == run_bootstrap_readiness.display_path(run_bootstrap_readiness.DEFAULT_OPEN_BLOCKERS_ROOT)
    )
    assert refresh["generated_at_utc"] == "2026-02-24T00:00:00Z"
    assert refresh["source"] == "fixture:bootstrap-readiness-refresh-fail"
    assert (
        refresh["open_blockers_json"]
        == run_bootstrap_readiness.display_path(open_blockers_path)
    )

    artifacts = summary["artifacts"]
    assert isinstance(artifacts, dict)
    assert artifacts["open_blockers_refresh_log"] == "extract_open_blockers.log"

    commands = summary["commands"]
    assert isinstance(commands, dict)
    assert list(commands.keys()) == [
        "extract_open_blockers_snapshot_json",
        "check_bootstrap_readiness_json",
        "check_bootstrap_readiness_markdown",
    ]
    assert commands["extract_open_blockers_snapshot_json"]["exit_code"] == 2

    errors = summary["errors"]
    assert isinstance(errors, list)
    assert any(
        "extract_open_blockers(snapshot-json) returned unexpected exit code 2" in entry
        for entry in errors
    )

    report_text = (output_dir / "bootstrap_readiness_report.md").read_text(encoding="utf-8")
    assert "- Exit code: `2`" in report_text
    assert "- generated_at_utc: `2026-02-24T00:00:00Z`" in report_text
    assert "- source: `fixture:bootstrap-readiness-refresh-fail`" in report_text
