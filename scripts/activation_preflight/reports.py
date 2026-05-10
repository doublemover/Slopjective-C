"""Activation preflight report and summary builders."""

from __future__ import annotations

from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.paths import display_path
from objc3c_tooling.subprocesses import python_script_command

from scripts.activation_preflight.contracts import CommandResult
from scripts.activation_preflight.payload_validation import bool_text

ACTIVATION_JSON_FILENAME = "check_activation_triggers.json"
ACTIVATION_MD_FILENAME = "check_activation_triggers.md"
SPEC_LINT_LOG_FILENAME = "spec_lint.log"
SNAPSHOT_CAPTURE_LOG_FILENAME = "capture_activation_snapshots.log"
OPEN_BLOCKERS_REFRESH_LOG_FILENAME = "extract_open_blockers.log"
SUMMARY_JSON_FILENAME = "activation_preflight_summary.json"
REPORT_MD_FILENAME = "activation_preflight_report.md"


def render_command_log(title: str, result: CommandResult) -> str:
    lines = [
        f"# {title}",
        "",
        "## stdout",
        "",
        result.stdout.rstrip("\n") if result.stdout else "_empty_",
        "",
        "## stderr",
        "",
        result.stderr.rstrip("\n") if result.stderr else "_empty_",
        "",
    ]
    return "\n".join(lines)


def render_spec_lint_log(result: CommandResult) -> str:
    return render_command_log("spec_lint command output", result)


def summarize_command(result: CommandResult) -> dict[str, Any]:
    return {
        "argv": python_script_command(display_path(result.spec.script_path), *result.spec.display_args),
        "exit_code": result.exit_code,
        "stdout_bytes": len(result.stdout.encode("utf-8")),
        "stderr_bytes": len(result.stderr.encode("utf-8")),
    }


def build_summary_payload(
    *,
    issues_path: Path,
    milestones_path: Path,
    catalog_path: Path,
    open_blockers_path: Path | None,
    output_dir: Path,
    spec_globs: Sequence[str],
    snapshot_refresh_requested: bool,
    snapshot_refresh_result: CommandResult | None,
    open_blockers_refresh_requested: bool,
    open_blockers_refresh_result: CommandResult | None,
    open_blockers_refresh_root: Path | None,
    open_blockers_refresh_generated_at_utc: str | None,
    open_blockers_refresh_source: str | None,
    issues_max_age_seconds: int | None,
    milestones_max_age_seconds: int | None,
    snapshot_generated_at_utc: str | None,
    activation_payload: dict[str, Any] | None,
    activation_json_result: CommandResult,
    activation_markdown_result: CommandResult,
    spec_lint_result: CommandResult,
    errors: Sequence[str],
    final_exit_code: int,
    final_status: str,
) -> dict[str, Any]:
    gate_open = None
    queue_state = None
    activation_required = None
    active_trigger_ids: list[str] = []
    activation_exit_code = activation_json_result.exit_code
    open_blocker_count = None
    open_blockers_trigger_fired = None

    if activation_payload is not None:
        gate_open = bool(activation_payload["gate_open"])
        queue_state = str(activation_payload["queue_state"])
        activation_required = bool(activation_payload["activation_required"])
        active_trigger_ids = [str(item) for item in activation_payload["active_trigger_ids"]]
        activation_exit_code = int(activation_payload.get("exit_code", activation_json_result.exit_code))
        open_blockers = activation_payload.get("open_blockers")
        if isinstance(open_blockers, dict):
            raw_open_blocker_count = open_blockers.get("count")
            raw_open_blockers_trigger_fired = open_blockers.get("trigger_fired")
            if isinstance(raw_open_blocker_count, int) and not isinstance(raw_open_blocker_count, bool):
                open_blocker_count = raw_open_blocker_count
            if isinstance(raw_open_blockers_trigger_fired, bool):
                open_blockers_trigger_fired = raw_open_blockers_trigger_fired

    commands: dict[str, Any] = {}
    if snapshot_refresh_result is not None:
        commands["capture_activation_snapshots"] = summarize_command(snapshot_refresh_result)
    if open_blockers_refresh_result is not None:
        commands["extract_open_blockers_snapshot_json"] = summarize_command(
            open_blockers_refresh_result
        )
    commands["check_activation_triggers_json"] = summarize_command(activation_json_result)
    commands["check_activation_triggers_markdown"] = summarize_command(activation_markdown_result)
    commands["spec_lint"] = summarize_command(spec_lint_result)

    return {
        "runner": "activation-preflight/v0.13",
        "inputs": {
            "issues_json": display_path(issues_path),
            "milestones_json": display_path(milestones_path),
            "catalog_json": display_path(catalog_path),
            "open_blockers_json": (
                display_path(open_blockers_path) if open_blockers_path is not None else None
            ),
            "open_blockers_refresh": open_blockers_refresh_requested,
            "open_blockers_root": (
                display_path(open_blockers_refresh_root)
                if open_blockers_refresh_root is not None
                else None
            ),
            "open_blockers_generated_at_utc": open_blockers_refresh_generated_at_utc,
            "open_blockers_source": open_blockers_refresh_source,
            "spec_globs": list(spec_globs),
            "snapshot_refresh": snapshot_refresh_requested,
            "issues_max_age_seconds": issues_max_age_seconds,
            "milestones_max_age_seconds": milestones_max_age_seconds,
            "snapshot_generated_at_utc": snapshot_generated_at_utc,
        },
        "artifacts": {
            "output_dir": display_path(output_dir),
            "check_activation_json": ACTIVATION_JSON_FILENAME,
            "check_activation_markdown": ACTIVATION_MD_FILENAME,
            "spec_lint_log": SPEC_LINT_LOG_FILENAME,
            "snapshot_capture_log": (
                SNAPSHOT_CAPTURE_LOG_FILENAME if snapshot_refresh_result is not None else None
            ),
            "open_blockers_refresh_log": (
                OPEN_BLOCKERS_REFRESH_LOG_FILENAME
                if open_blockers_refresh_result is not None
                else None
            ),
            "summary_json": SUMMARY_JSON_FILENAME,
            "report_markdown": REPORT_MD_FILENAME,
        },
        "snapshot": {
            "refresh_requested": snapshot_refresh_requested,
            "refresh_attempted": snapshot_refresh_result is not None,
            "refresh_exit_code": (
                snapshot_refresh_result.exit_code if snapshot_refresh_result is not None else None
            ),
            "open_blockers_refresh_requested": open_blockers_refresh_requested,
            "open_blockers_refresh_attempted": open_blockers_refresh_result is not None,
            "open_blockers_refresh_exit_code": (
                open_blockers_refresh_result.exit_code
                if open_blockers_refresh_result is not None
                else None
            ),
            "open_blockers_refresh_root": (
                display_path(open_blockers_refresh_root)
                if open_blockers_refresh_root is not None
                else None
            ),
            "open_blockers_generated_at_utc": open_blockers_refresh_generated_at_utc,
            "open_blockers_source": open_blockers_refresh_source,
            "issues_json": display_path(issues_path),
            "milestones_json": display_path(milestones_path),
            "open_blockers_json": (
                display_path(open_blockers_path) if open_blockers_path is not None else None
            ),
            "issues_max_age_seconds": issues_max_age_seconds,
            "milestones_max_age_seconds": milestones_max_age_seconds,
            "snapshot_generated_at_utc": snapshot_generated_at_utc,
        },
        "activation": {
            "gate_open": gate_open,
            "activation_required": activation_required,
            "queue_state": queue_state,
            "active_trigger_ids": active_trigger_ids,
            "open_blocker_count": open_blocker_count,
            "open_blockers_trigger_fired": open_blockers_trigger_fired,
            "exit_code": activation_exit_code,
        },
        "spec_lint": {
            "exit_code": spec_lint_result.exit_code,
            "ok": spec_lint_result.exit_code == 0,
        },
        "commands": commands,
        "errors": list(errors),
        "final_status": final_status,
        "final_exit_code": final_exit_code,
    }


def render_markdown_report(summary: dict[str, Any]) -> str:
    inputs = summary["inputs"]
    artifacts = summary["artifacts"]
    snapshot = summary["snapshot"]
    activation = summary["activation"]
    spec_lint = summary["spec_lint"]
    commands = summary["commands"]
    errors = summary["errors"]

    assert isinstance(inputs, dict)
    assert isinstance(artifacts, dict)
    assert isinstance(snapshot, dict)
    assert isinstance(activation, dict)
    assert isinstance(spec_lint, dict)
    assert isinstance(commands, dict)
    assert isinstance(errors, list)

    gate_open = activation["gate_open"]
    gate_open_text = "`unknown`"
    if isinstance(gate_open, bool):
        gate_open_text = f"`{bool_text(gate_open)}`"

    def optional_literal(value: Any) -> str:
        if value is None:
            return "_none_"
        return f"`{value}`"

    def optional_bool_literal(value: Any) -> str:
        if value is None:
            return "_none_"
        if isinstance(value, bool):
            return f"`{bool_text(value)}`"
        return f"`{value}`"

    lines = [
        "# Activation Preflight Orchestration",
        "",
        "## Inputs",
        "",
        f"- Issues snapshot: `{inputs['issues_json']}`",
        f"- Milestones snapshot: `{inputs['milestones_json']}`",
        f"- Catalog JSON: `{inputs['catalog_json']}`",
        "- Open blockers JSON: " + optional_literal(inputs["open_blockers_json"]),
        "- Open blockers refresh requested: "
        + optional_bool_literal(inputs["open_blockers_refresh"]),
        "- Open blockers scan root: " + optional_literal(inputs["open_blockers_root"]),
        "- Open blockers generated_at_utc: "
        + optional_literal(inputs["open_blockers_generated_at_utc"]),
        "- Open blockers source: " + optional_literal(inputs["open_blockers_source"]),
        (
            "- Spec lint globs: "
            + (
                ", ".join(f"`{glob}`" for glob in inputs["spec_globs"])
                if inputs["spec_globs"]
                else "_default spec_lint globs_"
            )
        ),
        "",
        "## Snapshot Refresh + Freshness",
        "",
        f"- Snapshot refresh requested: `{bool_text(bool(snapshot['refresh_requested']))}`",
        f"- Snapshot refresh attempted: `{bool_text(bool(snapshot['refresh_attempted']))}`",
        f"- Snapshot refresh exit code: {optional_literal(snapshot['refresh_exit_code'])}",
        (
            "- Open blockers refresh requested: "
            f"`{bool_text(bool(snapshot['open_blockers_refresh_requested']))}`"
        ),
        (
            "- Open blockers refresh attempted: "
            f"`{bool_text(bool(snapshot['open_blockers_refresh_attempted']))}`"
        ),
        (
            "- Open blockers refresh exit code: "
            + optional_literal(snapshot["open_blockers_refresh_exit_code"])
        ),
        "- Open blockers refresh root: " + optional_literal(snapshot["open_blockers_refresh_root"]),
        (
            "- Open blockers refresh generated_at_utc: "
            + optional_literal(snapshot["open_blockers_generated_at_utc"])
        ),
        (
            "- Open blockers refresh source: "
            + optional_literal(snapshot["open_blockers_source"])
        ),
        f"- Issues snapshot path: `{snapshot['issues_json']}`",
        f"- Milestones snapshot path: `{snapshot['milestones_json']}`",
        f"- Open blockers snapshot path: {optional_literal(snapshot['open_blockers_json'])}",
        f"- Issues max age seconds: {optional_literal(snapshot['issues_max_age_seconds'])}",
        f"- Milestones max age seconds: {optional_literal(snapshot['milestones_max_age_seconds'])}",
        f"- Snapshot generated_at_utc override: {optional_literal(snapshot['snapshot_generated_at_utc'])}",
        "",
        "## Activation State",
        "",
        f"- Gate open: {gate_open_text}",
        f"- Activation required: `{activation['activation_required']}`",
        f"- Queue state: `{activation['queue_state']}`",
        (
            "- Active trigger IDs: "
            + (
                ", ".join(f"`{item}`" for item in activation["active_trigger_ids"])
                if activation["active_trigger_ids"]
                else "_none_"
            )
        ),
        f"- Open blocker count: {optional_literal(activation['open_blocker_count'])}",
        (
            "- Open blockers trigger fired: "
            + optional_bool_literal(activation["open_blockers_trigger_fired"])
        ),
        f"- Activation checker exit code: `{activation['exit_code']}`",
        "",
        "## Spec Lint",
        "",
        f"- spec_lint exit code: `{spec_lint['exit_code']}`",
        f"- spec_lint ok: `{bool_text(bool(spec_lint['ok']))}`",
        "",
        "## Final Outcome",
        "",
        f"- Final status: `{summary['final_status']}`",
        f"- Final exit code: `{summary['final_exit_code']}`",
        "",
        "## Artifacts",
        "",
        f"- Output directory: `{artifacts['output_dir']}`",
        f"- Activation JSON: `{artifacts['check_activation_json']}`",
        f"- Activation markdown: `{artifacts['check_activation_markdown']}`",
        f"- spec_lint log: `{artifacts['spec_lint_log']}`",
        "- Snapshot capture log: "
        + (
            f"`{artifacts['snapshot_capture_log']}`"
            if artifacts["snapshot_capture_log"] is not None
            else "_none_"
        ),
        "- Open blockers refresh log: "
        + (
            f"`{artifacts['open_blockers_refresh_log']}`"
            if artifacts["open_blockers_refresh_log"] is not None
            else "_none_"
        ),
        f"- Summary JSON: `{artifacts['summary_json']}`",
        f"- Report markdown: `{artifacts['report_markdown']}`",
        "",
        "## Command Exit Codes",
        "",
        "| Command | Exit Code |",
        "| --- | --- |",
    ]

    for key, payload in commands.items():
        assert isinstance(payload, dict)
        lines.append(f"| `{key}` | `{payload['exit_code']}` |")

    lines.extend(["", "## Errors", ""])
    if errors:
        for entry in errors:
            lines.append(f"- {entry}")
    else:
        lines.append("- _none_")

    return "\n".join(lines).rstrip() + "\n"


__all__ = [
    "ACTIVATION_JSON_FILENAME",
    "ACTIVATION_MD_FILENAME",
    "OPEN_BLOCKERS_REFRESH_LOG_FILENAME",
    "REPORT_MD_FILENAME",
    "SNAPSHOT_CAPTURE_LOG_FILENAME",
    "SPEC_LINT_LOG_FILENAME",
    "SUMMARY_JSON_FILENAME",
    "build_summary_payload",
    "render_command_log",
    "render_markdown_report",
    "render_spec_lint_log",
]
