from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "run_activation_preflight.py"
SPEC = importlib.util.spec_from_file_location("run_activation_preflight", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/run_activation_preflight.py for tests.")
run_activation_preflight = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = run_activation_preflight
SPEC.loader.exec_module(run_activation_preflight)

REPO_ROOT = Path(__file__).resolve().parents[2]
FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "activation_triggers" / "preflight"


def run_scenario(tmp_path: Path, name: str) -> tuple[int, Path]:
    scenario_root = FIXTURE_ROOT / name
    output_dir = tmp_path / f"{name}_artifacts"
    open_blockers_path = scenario_root / "open_blockers.json"
    spec_glob = (scenario_root / "spec.md").resolve().relative_to(REPO_ROOT).as_posix()
    args = [
        "--issues-json",
        str(scenario_root / "issues.json"),
        "--milestones-json",
        str(scenario_root / "milestones.json"),
        "--catalog-json",
        str(scenario_root / "catalog.json"),
        "--spec-glob",
        spec_glob,
        "--output-dir",
        str(output_dir),
    ]
    if open_blockers_path.exists():
        args.extend(["--open-blockers-json", str(open_blockers_path)])
    code = run_activation_preflight.main(args)
    return code, output_dir


def read_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def bool_text(value: bool) -> str:
    return "true" if value else "false"


def markdown_freshness_cell(raw_value: object) -> str:
    if raw_value is None:
        return "_none_"
    if isinstance(raw_value, bool):
        return f"`{bool_text(raw_value)}`"
    return f"`{raw_value}`"


def render_checker_markdown(payload: dict[str, object]) -> str:
    inputs = payload["inputs"]
    assert isinstance(inputs, dict)
    actionable_statuses = payload["actionable_statuses"]
    assert isinstance(actionable_statuses, list)
    freshness_payload = payload["freshness"]
    assert isinstance(freshness_payload, dict)
    issues_freshness = freshness_payload["issues"]
    milestones_freshness = freshness_payload["milestones"]
    assert isinstance(issues_freshness, dict)
    assert isinstance(milestones_freshness, dict)
    t4_overlay = payload["t4_governance_overlay"]
    assert isinstance(t4_overlay, dict)
    open_blockers = payload["open_blockers"]
    assert isinstance(open_blockers, dict)
    triggers = payload["triggers"]
    assert isinstance(triggers, list)
    active_trigger_ids = payload["active_trigger_ids"]
    assert isinstance(active_trigger_ids, list)
    open_blockers_input = inputs["open_blockers_json"]
    open_blockers_input_text = (
        f"`{open_blockers_input}`" if open_blockers_input is not None else "_none_"
    )
    t4_overlay_input = inputs["t4_governance_overlay_json"]
    t4_overlay_input_text = (
        f"`{t4_overlay_input}`" if t4_overlay_input is not None else "_none_"
    )

    lines = [
        "# Activation Trigger Check",
        "",
        f"- Mode: `{payload['mode']}`",
        f"- Issues snapshot: `{inputs['issues_json']}`",
        f"- Milestones snapshot: `{inputs['milestones_json']}`",
        f"- Catalog JSON: `{inputs['catalog_json']}`",
        f"- Open blockers JSON: {open_blockers_input_text}",
        f"- T4 governance overlay JSON: {t4_overlay_input_text}",
        (
            "- Actionable statuses: "
            + ", ".join(f"`{status}`" for status in actionable_statuses)
        ),
        f"- Activation required: `{bool_text(bool(payload['activation_required']))}`",
        f"- T4 new scope publish: `{bool_text(bool(t4_overlay['new_scope_publish']))}`",
        f"- T4 source: `{t4_overlay['source']}`",
        f"- Gate open: `{bool_text(bool(payload['gate_open']))}`",
        f"- Queue state: `{payload['queue_state']}`",
        f"- Exit code: `{payload['exit_code']}`",
        f"- Open blockers count: `{open_blockers['count']}`",
        f"- Open blockers trigger fired: `{bool_text(bool(open_blockers['trigger_fired']))}`",
        "",
        "## Snapshot Freshness",
        "",
        "| Snapshot | Requested | Max age (s) | Generated at UTC | Age (s) | Fresh |",
        "| --- | --- | --- | --- | --- | --- |",
        (
            f"| Issues | {markdown_freshness_cell(issues_freshness.get('requested'))} | "
            f"{markdown_freshness_cell(issues_freshness.get('max_age_seconds'))} | "
            f"{markdown_freshness_cell(issues_freshness.get('generated_at_utc'))} | "
            f"{markdown_freshness_cell(issues_freshness.get('age_seconds'))} | "
            f"{markdown_freshness_cell(issues_freshness.get('fresh'))} |"
        ),
        (
            f"| Milestones | {markdown_freshness_cell(milestones_freshness.get('requested'))} | "
            f"{markdown_freshness_cell(milestones_freshness.get('max_age_seconds'))} | "
            f"{markdown_freshness_cell(milestones_freshness.get('generated_at_utc'))} | "
            f"{markdown_freshness_cell(milestones_freshness.get('age_seconds'))} | "
            f"{markdown_freshness_cell(milestones_freshness.get('fresh'))} |"
        ),
        "",
        "## Trigger Results",
        "",
        "| Trigger ID | Fired | Count | Condition |",
        "| --- | --- | --- | --- |",
    ]
    for entry in triggers:
        assert isinstance(entry, dict)
        lines.append(
            "| "
            f"`{entry['id']}` | "
            f"`{bool_text(bool(entry['fired']))}` | "
            f"{entry['count']} | "
            f"{entry['condition']} |"
        )

    lines.append("")
    if active_trigger_ids:
        lines.append(
            "- Active triggers: "
            + ", ".join(f"`{trigger_id}`" for trigger_id in active_trigger_ids)
        )
    else:
        lines.append("- Active triggers: _none_")
    return "\n".join(lines) + "\n"


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
