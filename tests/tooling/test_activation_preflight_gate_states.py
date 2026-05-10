from __future__ import annotations

from pathlib import Path

from activation_preflight_support import read_json, run_scenario


def test_runner_persists_artifacts_for_zero_open_and_returns_zero(tmp_path: Path) -> None:
    code, output_dir = run_scenario(tmp_path, "zero_open")

    assert code == 0

    activation_json_path = output_dir / "check_activation_triggers.json"
    activation_md_path = output_dir / "check_activation_triggers.md"
    spec_lint_log_path = output_dir / "spec_lint.log"
    summary_path = output_dir / "activation_preflight_summary.json"
    report_path = output_dir / "activation_preflight_report.md"
    for artifact in (
        activation_json_path,
        activation_md_path,
        spec_lint_log_path,
        summary_path,
        report_path,
    ):
        assert artifact.exists()
        assert artifact.is_file()
        assert b"\r" not in artifact.read_bytes()

    activation_payload = read_json(activation_json_path)
    assert activation_payload["gate_open"] is False
    assert activation_payload["activation_required"] is False
    assert activation_payload["queue_state"] == "idle"
    assert activation_payload["active_trigger_ids"] == []
    open_blockers = activation_payload["open_blockers"]
    assert isinstance(open_blockers, dict)
    assert open_blockers["count"] == 0
    assert open_blockers["trigger_fired"] is False

    summary = read_json(summary_path)
    assert list(summary.keys()) == [
        "runner",
        "inputs",
        "artifacts",
        "snapshot",
        "activation",
        "spec_lint",
        "commands",
        "errors",
        "final_status",
        "final_exit_code",
    ]
    assert summary["final_status"] == "ok"
    assert summary["final_exit_code"] == 0
    inputs = summary["inputs"]
    assert isinstance(inputs, dict)
    assert (
        inputs["open_blockers_json"]
        == "tests/tooling/fixtures/activation_triggers/preflight/zero_open/open_blockers.json"
    )
    activation = summary["activation"]
    assert isinstance(activation, dict)
    assert activation["gate_open"] is False
    assert activation["activation_required"] is False
    assert activation["queue_state"] == "idle"
    assert activation["active_trigger_ids"] == []
    assert activation["open_blocker_count"] == 0
    assert activation["open_blockers_trigger_fired"] is False
    assert activation["exit_code"] == 0

    snapshot = summary["snapshot"]
    assert isinstance(snapshot, dict)
    assert snapshot["refresh_requested"] is False
    assert snapshot["refresh_attempted"] is False
    assert snapshot["refresh_exit_code"] is None
    assert snapshot["snapshot_generated_at_utc"] is None
    assert snapshot["issues_json"] == "tests/tooling/fixtures/activation_triggers/preflight/zero_open/issues.json"
    assert (
        snapshot["milestones_json"]
        == "tests/tooling/fixtures/activation_triggers/preflight/zero_open/milestones.json"
    )
    assert (
        snapshot["open_blockers_json"]
        == "tests/tooling/fixtures/activation_triggers/preflight/zero_open/open_blockers.json"
    )
    assert snapshot["issues_max_age_seconds"] is None
    assert snapshot["milestones_max_age_seconds"] is None

    artifacts = summary["artifacts"]
    assert isinstance(artifacts, dict)
    assert artifacts["snapshot_capture_log"] is None

    spec_lint = summary["spec_lint"]
    assert isinstance(spec_lint, dict)
    assert spec_lint["ok"] is True
    assert spec_lint["exit_code"] == 0

    commands = summary["commands"]
    assert isinstance(commands, dict)
    assert list(commands.keys()) == [
        "check_activation_triggers_json",
        "check_activation_triggers_markdown",
        "spec_lint",
    ]
    assert commands["check_activation_triggers_json"]["exit_code"] == 0
    assert commands["check_activation_triggers_markdown"]["exit_code"] == 0
    assert commands["spec_lint"]["exit_code"] == 0

    report_text = report_path.read_text(encoding="utf-8")
    assert "- Gate open: `false`" in report_text
    assert (
        "- Open blockers JSON: "
        "`tests/tooling/fixtures/activation_triggers/preflight/zero_open/open_blockers.json`"
        in report_text
    )
    assert "- Open blocker count: `0`" in report_text
    assert "- Open blockers trigger fired: `false`" in report_text
    assert "- Snapshot capture log: _none_" in report_text
    assert "- Final status: `ok`" in report_text
    assert "- Final exit code: `0`" in report_text


def test_runner_propagates_gate_open_exit_code_deterministically(tmp_path: Path) -> None:
    code, output_dir = run_scenario(tmp_path, "gate_open")

    assert code == 1

    activation_json_path = output_dir / "check_activation_triggers.json"
    activation_md_path = output_dir / "check_activation_triggers.md"
    summary_path = output_dir / "activation_preflight_summary.json"
    report_path = output_dir / "activation_preflight_report.md"

    activation_payload = read_json(activation_json_path)
    assert activation_payload["gate_open"] is True
    assert activation_payload["activation_required"] is True
    assert activation_payload["queue_state"] == "dispatch-open"
    assert activation_payload["active_trigger_ids"] == ["T1-ISSUES"]
    open_blockers = activation_payload["open_blockers"]
    assert isinstance(open_blockers, dict)
    assert open_blockers["count"] == 0
    assert open_blockers["trigger_fired"] is False
    assert activation_payload["exit_code"] == 1

    markdown_text = activation_md_path.read_text(encoding="utf-8")
    assert "- Gate open: `true`" in markdown_text
    assert "- Queue state: `dispatch-open`" in markdown_text
    assert "- Open blockers count: `0`" in markdown_text

    summary = read_json(summary_path)
    assert summary["final_status"] == "activation-open"
    assert summary["final_exit_code"] == 1
    inputs = summary["inputs"]
    assert isinstance(inputs, dict)
    assert (
        inputs["open_blockers_json"]
        == "tests/tooling/fixtures/activation_triggers/preflight/gate_open/open_blockers.json"
    )
    activation = summary["activation"]
    assert isinstance(activation, dict)
    assert activation["gate_open"] is True
    assert activation["activation_required"] is True
    assert activation["queue_state"] == "dispatch-open"
    assert activation["active_trigger_ids"] == ["T1-ISSUES"]
    assert activation["open_blocker_count"] == 0
    assert activation["open_blockers_trigger_fired"] is False
    assert activation["exit_code"] == 1

    snapshot = summary["snapshot"]
    assert isinstance(snapshot, dict)
    assert snapshot["refresh_requested"] is False
    assert snapshot["refresh_attempted"] is False
    assert snapshot["refresh_exit_code"] is None
    assert (
        snapshot["open_blockers_json"]
        == "tests/tooling/fixtures/activation_triggers/preflight/gate_open/open_blockers.json"
    )

    spec_lint = summary["spec_lint"]
    assert isinstance(spec_lint, dict)
    assert spec_lint["ok"] is True
    assert spec_lint["exit_code"] == 0

    commands = summary["commands"]
    assert isinstance(commands, dict)
    assert commands["check_activation_triggers_json"]["exit_code"] == 1
    assert commands["check_activation_triggers_markdown"]["exit_code"] == 1
    assert commands["spec_lint"]["exit_code"] == 0

    report_text = report_path.read_text(encoding="utf-8")
    assert "- Gate open: `true`" in report_text
    assert (
        "- Open blockers JSON: "
        "`tests/tooling/fixtures/activation_triggers/preflight/gate_open/open_blockers.json`"
        in report_text
    )
    assert "- Open blocker count: `0`" in report_text
    assert "- Open blockers trigger fired: `false`" in report_text
    assert "- Final status: `activation-open`" in report_text
    assert "- Final exit code: `1`" in report_text


def test_runner_propagates_open_blockers_gate_open_deterministically(tmp_path: Path) -> None:
    code, output_dir = run_scenario(tmp_path, "blockers_gate_open")

    assert code == 1

    activation_json_path = output_dir / "check_activation_triggers.json"
    summary_path = output_dir / "activation_preflight_summary.json"
    report_path = output_dir / "activation_preflight_report.md"

    activation_payload = read_json(activation_json_path)
    assert activation_payload["gate_open"] is True
    assert activation_payload["activation_required"] is True
    assert activation_payload["queue_state"] == "dispatch-open"
    assert activation_payload["active_trigger_ids"] == ["T5-OPEN-BLOCKERS"]
    open_blockers = activation_payload["open_blockers"]
    assert isinstance(open_blockers, dict)
    assert open_blockers["count"] == 2
    assert open_blockers["trigger_fired"] is True
    assert activation_payload["exit_code"] == 1

    summary = read_json(summary_path)
    assert summary["final_status"] == "activation-open"
    assert summary["final_exit_code"] == 1
    inputs = summary["inputs"]
    assert isinstance(inputs, dict)
    assert (
        inputs["open_blockers_json"]
        == "tests/tooling/fixtures/activation_triggers/preflight/blockers_gate_open/open_blockers.json"
    )
    activation = summary["activation"]
    assert isinstance(activation, dict)
    assert activation["active_trigger_ids"] == ["T5-OPEN-BLOCKERS"]
    assert activation["open_blocker_count"] == 2
    assert activation["open_blockers_trigger_fired"] is True

    report_text = report_path.read_text(encoding="utf-8")
    assert (
        "- Open blockers JSON: "
        "`tests/tooling/fixtures/activation_triggers/preflight/blockers_gate_open/open_blockers.json`"
        in report_text
    )
    assert "- Open blocker count: `2`" in report_text
    assert "- Open blockers trigger fired: `true`" in report_text
    assert "- Final status: `activation-open`" in report_text
