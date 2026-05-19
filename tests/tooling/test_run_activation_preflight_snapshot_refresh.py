from __future__ import annotations

import json
from pathlib import Path

from activation_preflight_support import (
    FIXTURE_ROOT,
    read_json,
    render_checker_markdown,
    run_activation_preflight,
)

def test_runner_wires_snapshot_refresh_and_freshness_flags(
    monkeypatch: object, tmp_path: Path
) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    output_dir = tmp_path / "snapshot_artifacts"
    issues_path.write_text("[]\n", encoding="utf-8")
    milestones_path.write_text("[]\n", encoding="utf-8")
    catalog_path.write_text("{\"tasks\": []}\n", encoding="utf-8")

    activation_payload = {
        "mode": "offline-deterministic",
        "inputs": {
            "issues_json": issues_path.as_posix(),
            "milestones_json": milestones_path.as_posix(),
            "catalog_json": catalog_path.as_posix(),
            "open_blockers_json": None,
            "t4_governance_overlay_json": None,
        },
        "actionable_statuses": ["open", "open-blocked", "blocked"],
        "freshness": {
            "issues": {
                "requested": True,
                "max_age_seconds": 600,
                "generated_at_utc": "2026-02-23T22:00:00Z",
                "age_seconds": 0,
                "fresh": True,
            },
            "milestones": {
                "requested": True,
                "max_age_seconds": 600,
                "generated_at_utc": "2026-02-23T22:00:00Z",
                "age_seconds": 0,
                "fresh": True,
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

    def fake_run_command(
        spec: run_activation_preflight.CommandSpec,
    ) -> run_activation_preflight.CommandResult:
        command_specs.append(spec)
        if spec.name == "capture_activation_snapshots":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=0,
                stdout=(
                    "capture-activation-snapshots: OK "
                    "(issues=0, milestones=0, issues_output=issues.json, milestones_output=milestones.json)\n"
                ),
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
            "--refresh-snapshots",
            "--snapshot-generated-at-utc",
            "2026-02-23T22:00:00Z",
            "--issues-max-age-seconds",
            "600",
            "--milestones-max-age-seconds",
            "600",
            "--output-dir",
            str(output_dir),
        ]
    )

    assert code == 0
    assert [spec.name for spec in command_specs] == [
        "capture_activation_snapshots",
        "check_activation_triggers_json",
        "check_activation_triggers_markdown",
        "spec_lint",
    ]

    capture_spec = command_specs[0]
    assert "--issues-output" in capture_spec.display_args
    assert "--milestones-output" in capture_spec.display_args
    assert "--generated-at-utc" in capture_spec.display_args

    checker_spec = command_specs[1]
    assert "--issues-max-age-seconds" in checker_spec.display_args
    assert "--milestones-max-age-seconds" in checker_spec.display_args

    summary = read_json(output_dir / "activation_preflight_summary.json")
    inputs = summary["inputs"]
    assert isinstance(inputs, dict)
    assert inputs["open_blockers_json"] is None
    snapshot = summary["snapshot"]
    assert isinstance(snapshot, dict)
    assert snapshot["refresh_requested"] is True
    assert snapshot["refresh_attempted"] is True
    assert snapshot["refresh_exit_code"] == 0
    assert snapshot["open_blockers_json"] is None
    assert snapshot["issues_max_age_seconds"] == 600
    assert snapshot["milestones_max_age_seconds"] == 600
    assert snapshot["snapshot_generated_at_utc"] == "2026-02-23T22:00:00Z"
    activation = summary["activation"]
    assert isinstance(activation, dict)
    assert activation["open_blocker_count"] == 0
    assert activation["open_blockers_trigger_fired"] is False
    assert (output_dir / "capture_activation_snapshots.log").exists()



def test_runner_refresh_and_freshness_wiring_persists_snapshot_artifacts(
    tmp_path: Path,
    monkeypatch,
) -> None:
    scenario_root = FIXTURE_ROOT / "zero_open"
    issues_path = tmp_path / "refreshed" / "issues.json"
    milestones_path = tmp_path / "refreshed" / "milestones.json"
    output_dir = tmp_path / "artifacts"

    capture_script_path = tmp_path / "capture_activation_snapshots.py"
    capture_script_path.write_text("# mock script path marker\n", encoding="utf-8")

    issued_specs: dict[str, run_activation_preflight.CommandSpec] = {}

    def fake_run_command(spec: run_activation_preflight.CommandSpec) -> run_activation_preflight.CommandResult:
        issued_specs[spec.name] = spec

        if spec.name == "capture_activation_snapshots":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=0,
                stdout=(
                    "capture-activation-snapshots: OK "
                    "(issues=0, milestones=0, issues_output=tmp/issues.json, "
                    "milestones_output=tmp/milestones.json)\n"
                ),
                stderr="",
            )

        if spec.name == "check_activation_triggers_json":
            payload = {
                "mode": "offline-deterministic",
                "inputs": {
                    "issues_json": run_activation_preflight.display_path(issues_path),
                    "milestones_json": run_activation_preflight.display_path(milestones_path),
                    "catalog_json": run_activation_preflight.display_path(
                        scenario_root / "catalog.json"
                    ),
                    "open_blockers_json": None,
                    "t4_governance_overlay_json": None,
                },
                "actionable_statuses": ["open", "open-blocked", "blocked"],
                "freshness": {
                    "issues": {
                        "requested": True,
                        "max_age_seconds": 120,
                        "generated_at_utc": "2026-02-23T00:00:00Z",
                        "age_seconds": 0,
                        "fresh": True,
                    },
                    "milestones": {
                        "requested": True,
                        "max_age_seconds": 600,
                        "generated_at_utc": "2026-02-23T00:00:00Z",
                        "age_seconds": 0,
                        "fresh": True,
                    },
                },
                "gate_open": False,
                "activation_required": False,
                "queue_state": "idle",
                "active_trigger_ids": [],
                "triggers": [
                    {
                        "id": "T1-ISSUES",
                        "condition": "open issues > 0",
                        "count": 0,
                        "fired": False,
                    },
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
                "t4_governance_overlay": {
                    "new_scope_publish": False,
                    "source": "default-false",
                },
                "open_blockers": {
                    "count": 0,
                    "trigger_id": "T5-OPEN-BLOCKERS",
                    "trigger_fired": False,
                },
                "exit_code": 0,
            }
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=0,
                stdout=json.dumps(payload) + "\n",
                stderr="",
            )

        if spec.name == "check_activation_triggers_markdown":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=0,
                stdout=render_checker_markdown(
                    {
                        "mode": "offline-deterministic",
                        "inputs": {
                            "issues_json": run_activation_preflight.display_path(issues_path),
                            "milestones_json": run_activation_preflight.display_path(
                                milestones_path
                            ),
                            "catalog_json": run_activation_preflight.display_path(
                                scenario_root / "catalog.json"
                            ),
                            "open_blockers_json": None,
                            "t4_governance_overlay_json": None,
                        },
                        "actionable_statuses": ["open", "open-blocked", "blocked"],
                        "freshness": {
                            "issues": {
                                "requested": True,
                                "max_age_seconds": 120,
                                "generated_at_utc": "2026-02-23T00:00:00Z",
                                "age_seconds": 0,
                                "fresh": True,
                            },
                            "milestones": {
                                "requested": True,
                                "max_age_seconds": 600,
                                "generated_at_utc": "2026-02-23T00:00:00Z",
                                "age_seconds": 0,
                                "fresh": True,
                            },
                        },
                        "activation_required": False,
                        "t4_governance_overlay": {
                            "new_scope_publish": False,
                            "source": "default-false",
                        },
                        "gate_open": False,
                        "queue_state": "idle",
                        "exit_code": 0,
                        "open_blockers": {
                            "count": 0,
                            "trigger_fired": False,
                        },
                        "triggers": [
                            {
                                "id": "T1-ISSUES",
                                "condition": "open issues > 0",
                                "count": 0,
                                "fired": False,
                            },
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
                    }
                ),
                stderr="",
            )

        if spec.name == "spec_lint":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=0,
                stdout="spec-lint: OK\n",
                stderr="",
            )

        raise AssertionError(f"unexpected command name: {spec.name}")

    monkeypatch.setattr(
        run_activation_preflight,
        "CAPTURE_SNAPSHOTS_SCRIPT_PATH",
        capture_script_path,
    )
    monkeypatch.setattr(run_activation_preflight, "run_command", fake_run_command)

    args = [
        "--issues-json",
        str(issues_path),
        "--milestones-json",
        str(milestones_path),
        "--catalog-json",
        str(scenario_root / "catalog.json"),
        "--refresh-snapshots",
        "--snapshot-generated-at-utc",
        "2026-02-23T00:00:00Z",
        "--issues-max-age-seconds",
        "120",
        "--milestones-max-age-seconds",
        "600",
        "--output-dir",
        str(output_dir),
    ]

    code = run_activation_preflight.main(args)
    assert code == 0

    capture_spec = issued_specs["capture_activation_snapshots"]
    assert capture_spec.actual_args == (
        "--issues-output",
        str(issues_path),
        "--milestones-output",
        str(milestones_path),
        "--generated-at-utc",
        "2026-02-23T00:00:00Z",
    )

    activation_json_spec = issued_specs["check_activation_triggers_json"]
    assert "--issues-max-age-seconds" in activation_json_spec.actual_args
    assert "120" in activation_json_spec.actual_args
    assert "--milestones-max-age-seconds" in activation_json_spec.actual_args
    assert "600" in activation_json_spec.actual_args

    summary_path = output_dir / "activation_preflight_summary.json"
    report_path = output_dir / "activation_preflight_report.md"
    capture_log_path = output_dir / "capture_activation_snapshots.log"

    assert summary_path.exists()
    assert report_path.exists()
    assert capture_log_path.exists()

    summary = read_json(summary_path)
    inputs = summary["inputs"]
    assert isinstance(inputs, dict)
    assert inputs["open_blockers_json"] is None
    snapshot = summary["snapshot"]
    assert isinstance(snapshot, dict)
    assert snapshot["refresh_requested"] is True
    assert snapshot["refresh_attempted"] is True
    assert snapshot["refresh_exit_code"] == 0
    assert snapshot["snapshot_generated_at_utc"] == "2026-02-23T00:00:00Z"
    assert snapshot["issues_json"] == run_activation_preflight.display_path(issues_path)
    assert snapshot["milestones_json"] == run_activation_preflight.display_path(
        milestones_path
    )
    assert snapshot["open_blockers_json"] is None
    assert snapshot["issues_max_age_seconds"] == 120
    assert snapshot["milestones_max_age_seconds"] == 600
    activation = summary["activation"]
    assert isinstance(activation, dict)
    assert activation["open_blocker_count"] == 0
    assert activation["open_blockers_trigger_fired"] is False

    artifacts = summary["artifacts"]
    assert isinstance(artifacts, dict)
    assert artifacts["snapshot_capture_log"] == "capture_activation_snapshots.log"

    commands = summary["commands"]
    assert isinstance(commands, dict)
    assert commands["capture_activation_snapshots"]["exit_code"] == 0

    report_text = report_path.read_text(encoding="utf-8")
    assert "- Open blockers JSON: _none_" in report_text
    assert "- Open blocker count: `0`" in report_text
    assert "- Open blockers trigger fired: `false`" in report_text
    assert "- Snapshot refresh requested: `true`" in report_text
    assert "- Snapshot generated_at_utc override: `2026-02-23T00:00:00Z`" in report_text
    assert "- Issues max age seconds: `120`" in report_text
    assert "- Milestones max age seconds: `600`" in report_text
    assert "- Snapshot capture log: `capture_activation_snapshots.log`" in report_text
