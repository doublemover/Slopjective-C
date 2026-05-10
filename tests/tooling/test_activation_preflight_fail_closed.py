from __future__ import annotations

import json
from pathlib import Path

from activation_preflight_support import FIXTURE_ROOT, read_json, run_activation_preflight


def test_runner_fails_closed_on_activation_reduction_drift(tmp_path: Path, monkeypatch) -> None:
    scenario_root = FIXTURE_ROOT / "zero_open"
    output_dir = tmp_path / "drift_artifacts"

    def fake_run_command(spec: run_activation_preflight.CommandSpec) -> run_activation_preflight.CommandResult:
        if spec.name == "check_activation_triggers_json":
            payload = {
                "mode": "offline-deterministic",
                "inputs": {
                    "issues_json": run_activation_preflight.display_path(
                        scenario_root / "issues.json"
                    ),
                    "milestones_json": run_activation_preflight.display_path(
                        scenario_root / "milestones.json"
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
                "gate_open": True,
                "queue_state": "dispatch-open",
                "exit_code": 1,
            }
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=1,
                stdout=json.dumps(payload, indent=2) + "\n",
                stderr="",
            )

        if spec.name == "check_activation_triggers_markdown":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=1,
                stdout=(
                    "# Activation Trigger Check\n"
                    "- Activation required: `false`\n"
                    "- T4 new scope publish: `false`\n"
                    "- T4 source: `default-false`\n"
                    "- Gate open: `true`\n"
                    "- Queue state: `dispatch-open`\n"
                    "- Exit code: `1`\n"
                    "- Open blockers count: `0`\n"
                    "- Open blockers trigger fired: `false`\n"
                    "## Trigger Results\n"
                    "| Trigger ID | Fired | Count | Condition |\n"
                    "- Active triggers: _none_\n"
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

    monkeypatch.setattr(run_activation_preflight, "run_command", fake_run_command)

    code = run_activation_preflight.main(
        [
            "--issues-json",
            str(scenario_root / "issues.json"),
            "--milestones-json",
            str(scenario_root / "milestones.json"),
            "--catalog-json",
            str(scenario_root / "catalog.json"),
            "--output-dir",
            str(output_dir),
        ]
    )

    assert code == 2
    summary = read_json(output_dir / "activation_preflight_summary.json")
    assert summary["final_status"] == "runner-error"
    assert summary["final_exit_code"] == 2
    errors = summary["errors"]
    assert isinstance(errors, list)
    assert any("gate reduction mismatch" in entry for entry in errors)


def test_runner_fails_closed_on_checker_input_provenance_drift(
    tmp_path: Path, monkeypatch
) -> None:
    scenario_root = FIXTURE_ROOT / "zero_open"
    output_dir = tmp_path / "provenance_drift_artifacts"

    def fake_run_command(spec: run_activation_preflight.CommandSpec) -> run_activation_preflight.CommandResult:
        if spec.name == "check_activation_triggers_json":
            payload = {
                "mode": "offline-deterministic",
                "inputs": {
                    "issues_json": "drift/issues.json",
                    "milestones_json": run_activation_preflight.display_path(
                        scenario_root / "milestones.json"
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
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=0,
                stdout=json.dumps(payload, indent=2) + "\n",
                stderr="",
            )

        if spec.name == "check_activation_triggers_markdown":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=0,
                stdout="# Activation Trigger Check\n",
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

    monkeypatch.setattr(run_activation_preflight, "run_command", fake_run_command)

    code = run_activation_preflight.main(
        [
            "--issues-json",
            str(scenario_root / "issues.json"),
            "--milestones-json",
            str(scenario_root / "milestones.json"),
            "--catalog-json",
            str(scenario_root / "catalog.json"),
            "--output-dir",
            str(output_dir),
        ]
    )

    assert code == 2
    summary = read_json(output_dir / "activation_preflight_summary.json")
    assert summary["final_status"] == "runner-error"
    errors = summary["errors"]
    assert isinstance(errors, list)
    assert any("inputs provenance drift" in entry for entry in errors)


def test_runner_fails_closed_when_checker_returns_hard_failure(
    tmp_path: Path, monkeypatch
) -> None:
    scenario_root = FIXTURE_ROOT / "zero_open"
    output_dir = tmp_path / "checker_hard_fail_artifacts"

    def fake_run_command(spec: run_activation_preflight.CommandSpec) -> run_activation_preflight.CommandResult:
        if spec.name == "check_activation_triggers_json":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=2,
                stdout="",
                stderr="error: open issues snapshot freshness check failed\n",
            )

        if spec.name == "check_activation_triggers_markdown":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=2,
                stdout="",
                stderr="error: open issues snapshot freshness check failed\n",
            )

        if spec.name == "spec_lint":
            return run_activation_preflight.CommandResult(
                spec=spec,
                exit_code=0,
                stdout="spec-lint: OK\n",
                stderr="",
            )

        raise AssertionError(f"unexpected command name: {spec.name}")

    monkeypatch.setattr(run_activation_preflight, "run_command", fake_run_command)

    code = run_activation_preflight.main(
        [
            "--issues-json",
            str(scenario_root / "issues.json"),
            "--milestones-json",
            str(scenario_root / "milestones.json"),
            "--catalog-json",
            str(scenario_root / "catalog.json"),
            "--issues-max-age-seconds",
            "60",
            "--output-dir",
            str(output_dir),
        ]
    )

    assert code == 2
    summary = read_json(output_dir / "activation_preflight_summary.json")
    assert summary["final_status"] == "runner-error"
    assert summary["final_exit_code"] == 2
    commands = summary["commands"]
    assert isinstance(commands, dict)
    assert commands["check_activation_triggers_json"]["exit_code"] == 2
    errors = summary["errors"]
    assert isinstance(errors, list)
    assert any("returned unexpected exit code 2" in entry for entry in errors)
