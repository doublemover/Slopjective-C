from __future__ import annotations

import json
from pathlib import Path

from activation_preflight_support import (
    read_json,
    render_checker_markdown,
    run_activation_preflight,
)

def test_runner_wires_open_blocker_refresh_with_deterministic_default_output_path(
    monkeypatch: object, tmp_path: Path
) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    output_dir = tmp_path / "open_blockers_refresh_artifacts"
    open_blockers_path = output_dir / "inputs" / "open_blockers.snapshot.json"
    issues_path.write_text("[]\n", encoding="utf-8")
    milestones_path.write_text("[]\n", encoding="utf-8")
    catalog_path.write_text("{\"tasks\": []}\n", encoding="utf-8")

    blocker_snapshot = json.dumps(
        {
            "generated_at_utc": "2026-02-24T00:00:00Z",
            "source": "fixture:open-blocker-refresh",
            "open_blocker_count": 0,
            "open_blockers": [],
        },
        indent=2,
    ) + "\n"
    activation_payload = {
        "mode": "offline-deterministic",
        "inputs": {
            "issues_json": run_activation_preflight.display_path(issues_path),
            "milestones_json": run_activation_preflight.display_path(milestones_path),
            "catalog_json": run_activation_preflight.display_path(catalog_path),
            "open_blockers_json": run_activation_preflight.display_path(open_blockers_path),
            "t4_governance_overlay_json": None,
        },
        "actionable_statuses": ["open", "open-blocked", "blocked"],
        "freshness": {
            "issues": {
                "requested": False,
                "max_age_seconds": None,
                "generated_at_utc": None,
                "age_seconds": None,
                "fresh": None,
            },
            "milestones": {
                "requested": False,
                "max_age_seconds": None,
                "generated_at_utc": None,
                "age_seconds": None,
                "fresh": None,
            },
        },
        "triggers": [
            {"id": "T1-ISSUES", "condition": "open issues > 0", "count": 0, "fired": False},
            {
                "id": "T2-MILESTONES",
                "condition": "open milestones > 0",
                "count": 0,
                "fired": False,
            },
            {
                "id": "T3-ACTIONABLE-ROWS",
                "condition": "actionable catalog rows > 0",
                "count": 0,
                "fired": False,
            },
            {
                "id": "T5-OPEN-BLOCKERS",
                "condition": "open blockers > 0",
                "count": 0,
                "fired": False,
            },
        ],
        "active_trigger_ids": [],
        "activation_required": False,
        "open_blockers": {
            "count": 0,
            "trigger_id": "T5-OPEN-BLOCKERS",
            "trigger_fired": False,
        },
        "t4_governance_overlay": {"new_scope_publish": False, "source": "default-false"},
        "gate_open": False,
        "queue_state": "idle",
        "exit_code": 0,
    }
    activation_json = json.dumps(activation_payload, indent=2) + "\n"
    activation_md = render_checker_markdown(activation_payload)

    command_specs: list[run_activation_preflight.CommandSpec] = []

    def fake_run_command(spec: run_activation_preflight.CommandSpec) -> run_activation_preflight.CommandResult:
        command_specs.append(spec)
        if spec.name == "extract_open_blockers_snapshot_json":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=0,
                stdout=blocker_snapshot,
                stderr="",
            )
        if spec.name == "check_activation_triggers_json":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=0,
                stdout=activation_json,
                stderr="",
            )
        if spec.name == "check_activation_triggers_markdown":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=0,
                stdout=activation_md,
                stderr="",
            )
        if spec.name == "spec_lint":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=0,
                stdout="spec-lint: OK\n",
                stderr="",
            )
        raise AssertionError(f"unexpected command spec: {spec.name}")

    monkeypatch.setattr(run_activation_preflight, "run_command", fake_run_command)

    code = run_activation_preflight.main(
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
            "fixture:open-blocker-refresh",
            "--output-dir",
            str(output_dir),
        ]
    )

    assert code == 0
    assert [spec.name for spec in command_specs] == [
        "extract_open_blockers_snapshot_json",
        "check_activation_triggers_json",
        "check_activation_triggers_markdown",
        "spec_lint",
    ]

    refresh_spec = command_specs[0]
    assert refresh_spec.actual_args == (
        "--root",
        str(run_activation_preflight.DEFAULT_OPEN_BLOCKERS_ROOT),
        "--format",
        "snapshot-json",
        "--generated-at-utc",
        "2026-02-24T00:00:00Z",
        "--source",
        "fixture:open-blocker-refresh",
    )

    checker_spec = command_specs[1]
    assert checker_spec.actual_args == (
        "--issues-json",
        str(issues_path),
        "--milestones-json",
        str(milestones_path),
        "--catalog-json",
        str(catalog_path),
        "--open-blockers-json",
        str(open_blockers_path),
        "--format",
        "json",
    )

    assert open_blockers_path.read_text(encoding="utf-8") == blocker_snapshot
    assert (output_dir / "extract_open_blockers.log").exists()

    summary = read_json(output_dir / "activation_preflight_summary.json")
    inputs = summary["inputs"]
    assert isinstance(inputs, dict)
    assert (
        inputs["open_blockers_json"]
        == run_activation_preflight.display_path(open_blockers_path)
    )
    assert inputs["open_blockers_refresh"] is True
    assert (
        inputs["open_blockers_root"]
        == run_activation_preflight.display_path(run_activation_preflight.DEFAULT_OPEN_BLOCKERS_ROOT)
    )
    assert inputs["open_blockers_generated_at_utc"] == "2026-02-24T00:00:00Z"
    assert inputs["open_blockers_source"] == "fixture:open-blocker-refresh"

    snapshot = summary["snapshot"]
    assert isinstance(snapshot, dict)
    assert snapshot["open_blockers_refresh_requested"] is True
    assert snapshot["open_blockers_refresh_attempted"] is True
    assert snapshot["open_blockers_refresh_exit_code"] == 0
    assert (
        snapshot["open_blockers_refresh_root"]
        == run_activation_preflight.display_path(run_activation_preflight.DEFAULT_OPEN_BLOCKERS_ROOT)
    )
    assert snapshot["open_blockers_generated_at_utc"] == "2026-02-24T00:00:00Z"
    assert snapshot["open_blockers_source"] == "fixture:open-blocker-refresh"
    assert (
        snapshot["open_blockers_json"]
        == run_activation_preflight.display_path(open_blockers_path)
    )

    artifacts = summary["artifacts"]
    assert isinstance(artifacts, dict)
    assert artifacts["open_blockers_refresh_log"] == "extract_open_blockers.log"

    commands = summary["commands"]
    assert isinstance(commands, dict)
    assert list(commands.keys()) == [
        "extract_open_blockers_snapshot_json",
        "check_activation_triggers_json",
        "check_activation_triggers_markdown",
        "spec_lint",
    ]
    assert commands["extract_open_blockers_snapshot_json"]["exit_code"] == 0

    report_text = (output_dir / "activation_preflight_report.md").read_text(encoding="utf-8")
    assert "- Open blockers refresh requested: `true`" in report_text
    assert "- Open blockers refresh exit code: `0`" in report_text
    assert (
        "- Open blockers refresh root: "
        f"`{run_activation_preflight.display_path(run_activation_preflight.DEFAULT_OPEN_BLOCKERS_ROOT)}`"
        in report_text
    )
    assert "- Open blockers refresh log: `extract_open_blockers.log`" in report_text



def test_runner_fails_closed_when_open_blocker_refresh_fails(
    tmp_path: Path, monkeypatch
) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    output_dir = tmp_path / "open_blockers_refresh_fail_artifacts"
    open_blockers_path = output_dir / "inputs" / "open_blockers.snapshot.json"
    issues_path.write_text("[]\n", encoding="utf-8")
    milestones_path.write_text("[]\n", encoding="utf-8")
    catalog_path.write_text("{\"tasks\": []}\n", encoding="utf-8")

    activation_payload = {
        "mode": "offline-deterministic",
        "inputs": {
            "issues_json": run_activation_preflight.display_path(issues_path),
            "milestones_json": run_activation_preflight.display_path(milestones_path),
            "catalog_json": run_activation_preflight.display_path(catalog_path),
            "open_blockers_json": run_activation_preflight.display_path(open_blockers_path),
            "t4_governance_overlay_json": None,
        },
        "actionable_statuses": ["open", "open-blocked", "blocked"],
        "freshness": {
            "issues": {
                "requested": False,
                "max_age_seconds": None,
                "generated_at_utc": None,
                "age_seconds": None,
                "fresh": None,
            },
            "milestones": {
                "requested": False,
                "max_age_seconds": None,
                "generated_at_utc": None,
                "age_seconds": None,
                "fresh": None,
            },
        },
        "triggers": [
            {"id": "T1-ISSUES", "condition": "open issues > 0", "count": 0, "fired": False},
            {
                "id": "T2-MILESTONES",
                "condition": "open milestones > 0",
                "count": 0,
                "fired": False,
            },
            {
                "id": "T3-ACTIONABLE-ROWS",
                "condition": "actionable catalog rows > 0",
                "count": 0,
                "fired": False,
            },
            {
                "id": "T5-OPEN-BLOCKERS",
                "condition": "open blockers > 0",
                "count": 0,
                "fired": False,
            },
        ],
        "active_trigger_ids": [],
        "activation_required": False,
        "open_blockers": {
            "count": 0,
            "trigger_id": "T5-OPEN-BLOCKERS",
            "trigger_fired": False,
        },
        "t4_governance_overlay": {"new_scope_publish": False, "source": "default-false"},
        "gate_open": False,
        "queue_state": "idle",
        "exit_code": 0,
    }
    activation_json = json.dumps(activation_payload, indent=2) + "\n"
    activation_md = render_checker_markdown(activation_payload)

    def fake_run_command(spec: run_activation_preflight.CommandSpec) -> run_activation_preflight.CommandResult:
        if spec.name == "extract_open_blockers_snapshot_json":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=2,
                stdout="",
                stderr="error: malformed OPEN blocker row\n",
            )
        if spec.name == "check_activation_triggers_json":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=0,
                stdout=activation_json,
                stderr="",
            )
        if spec.name == "check_activation_triggers_markdown":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=0,
                stdout=activation_md,
                stderr="",
            )
        if spec.name == "spec_lint":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=0,
                stdout="spec-lint: OK\n",
                stderr="",
            )
        raise AssertionError(f"unexpected command spec: {spec.name}")

    monkeypatch.setattr(run_activation_preflight, "run_command", fake_run_command)

    code = run_activation_preflight.main(
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
            "fixture:open-blocker-refresh-fail",
            "--output-dir",
            str(output_dir),
        ]
    )

    assert code == 2

    summary = read_json(output_dir / "activation_preflight_summary.json")
    assert summary["final_status"] == "runner-error"
    assert summary["final_exit_code"] == 2

    snapshot = summary["snapshot"]
    assert isinstance(snapshot, dict)
    assert snapshot["open_blockers_refresh_requested"] is True
    assert snapshot["open_blockers_refresh_attempted"] is True
    assert snapshot["open_blockers_refresh_exit_code"] == 2
    assert (
        snapshot["open_blockers_json"]
        == run_activation_preflight.display_path(open_blockers_path)
    )

    commands = summary["commands"]
    assert isinstance(commands, dict)
    assert commands["extract_open_blockers_snapshot_json"]["exit_code"] == 2

    errors = summary["errors"]
    assert isinstance(errors, list)
    assert any(
        "extract_open_blockers(snapshot-json) returned unexpected exit code 2" in entry
        for entry in errors
    )

    assert (output_dir / "extract_open_blockers.log").exists()
    report_text = (output_dir / "activation_preflight_report.md").read_text(encoding="utf-8")
    assert "- Open blockers refresh exit code: `2`" in report_text


