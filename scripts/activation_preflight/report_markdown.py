"""Activation preflight markdown report rendering."""

from __future__ import annotations

from typing import Any

from scripts.activation_preflight.payload_validation import bool_text


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
    "optional_bool_literal",
    "optional_literal",
    "render_markdown_report",
]
