"""Markdown report validation for activation preflight checks."""

from __future__ import annotations

from typing import Any

from scripts.activation_preflight.contracts import CommandResult
from scripts.activation_preflight.payload_validation_constants import (
    EXIT_GATE_CLOSED,
    EXIT_GATE_OPEN,
)


def bool_text(value: bool) -> str:
    return "true" if value else "false"


def markdown_freshness_cell(raw_value: object) -> str:
    if raw_value is None:
        return "_none_"
    if isinstance(raw_value, bool):
        return f"`{bool_text(raw_value)}`"
    return f"`{raw_value}`"


def find_markdown_table_row(markdown_text: str, label: str) -> list[str] | None:
    row_prefix = f"| {label} |"
    for line in markdown_text.splitlines():
        if not line.startswith(row_prefix):
            continue
        cells = [cell.strip() for cell in line.split("|")[1:-1]]
        if len(cells) != 6:
            return []
        return cells
    return None


def validate_markdown_freshness_row(
    *,
    markdown_text: str,
    label: str,
    freshness_entry: dict[str, Any],
) -> str | None:
    cells = find_markdown_table_row(markdown_text, label)
    if cells is None:
        return (
            "check_activation_triggers(markdown) missing deterministic line "
            f"for freshness row {label!r}."
        )
    if not cells:
        return (
            "check_activation_triggers(markdown) malformed freshness row "
            f"for {label!r}: expected 6 columns."
        )

    expected_cells = (
        label,
        markdown_freshness_cell(freshness_entry.get("requested")),
        markdown_freshness_cell(freshness_entry.get("max_age_seconds")),
        markdown_freshness_cell(freshness_entry.get("generated_at_utc")),
        markdown_freshness_cell(freshness_entry.get("age_seconds")),
        markdown_freshness_cell(freshness_entry.get("fresh")),
    )
    if cells[0] != expected_cells[0]:
        return (
            "check_activation_triggers(markdown) freshness row drift for "
            f"{label!r}: label={cells[0]!r} expected={expected_cells[0]!r}."
        )
    if cells[1] != expected_cells[1]:
        return (
            "check_activation_triggers(markdown) freshness row drift for "
            f"{label!r}: requested={cells[1]!r} expected={expected_cells[1]!r}."
        )
    if cells[2] != expected_cells[2]:
        return (
            "check_activation_triggers(markdown) freshness row drift for "
            f"{label!r}: max_age_seconds={cells[2]!r} expected={expected_cells[2]!r}."
        )
    if cells[3] != expected_cells[3]:
        return (
            "check_activation_triggers(markdown) freshness row drift for "
            f"{label!r}: generated_at_utc={cells[3]!r} expected={expected_cells[3]!r}."
        )

    expected_age_cell = expected_cells[4]
    if expected_age_cell == "_none_":
        if cells[4] != "_none_":
            return (
                "check_activation_triggers(markdown) freshness row drift for "
                f"{label!r}: age_seconds={cells[4]!r} expected='_none_'."
            )
        if cells[5] != expected_cells[5]:
            return (
                "check_activation_triggers(markdown) freshness row drift for "
                f"{label!r}: fresh={cells[5]!r} expected={expected_cells[5]!r}."
            )
        return None

    if not (cells[4].startswith("`") and cells[4].endswith("`")):
        return (
            "check_activation_triggers(markdown) freshness row drift for "
            f"{label!r}: age_seconds cell {cells[4]!r} is not markdown-quoted."
        )
    age_text = cells[4][1:-1]
    if not age_text.isdigit():
        return (
            "check_activation_triggers(markdown) freshness row drift for "
            f"{label!r}: age_seconds={cells[4]!r} must be a non-negative integer."
        )
    if cells[5] != expected_cells[5]:
        return (
            "check_activation_triggers(markdown) freshness row drift for "
            f"{label!r}: fresh={cells[5]!r} expected={expected_cells[5]!r}."
        )
    return None


def check_markdown_gate_consistency(
    markdown_result: CommandResult,
    *,
    activation_payload: dict[str, Any],
) -> str | None:
    if markdown_result.exit_code not in (EXIT_GATE_CLOSED, EXIT_GATE_OPEN):
        return (
            "check_activation_triggers(markdown) returned unexpected exit code "
            f"{markdown_result.exit_code}."
        )

    gate_open = bool(activation_payload["gate_open"])
    activation_required = bool(activation_payload["activation_required"])
    queue_state = str(activation_payload["queue_state"])
    expected_exit = int(activation_payload["exit_code"])

    t4_overlay = activation_payload["t4_governance_overlay"]
    assert isinstance(t4_overlay, dict)
    t4_new_scope_publish = bool(t4_overlay["new_scope_publish"])
    t4_source = str(t4_overlay["source"])

    open_blockers = activation_payload["open_blockers"]
    assert isinstance(open_blockers, dict)
    open_blockers_count = int(open_blockers["count"])
    open_blockers_trigger_fired = bool(open_blockers["trigger_fired"])

    inputs = activation_payload["inputs"]
    assert isinstance(inputs, dict)
    open_blockers_input = inputs.get("open_blockers_json")
    open_blockers_input_text = (
        f"`{open_blockers_input}`" if open_blockers_input is not None else "_none_"
    )
    t4_overlay_input = inputs.get("t4_governance_overlay_json")
    t4_overlay_input_text = (
        f"`{t4_overlay_input}`" if t4_overlay_input is not None else "_none_"
    )

    freshness = activation_payload["freshness"]
    assert isinstance(freshness, dict)
    issues_freshness = freshness.get("issues")
    milestones_freshness = freshness.get("milestones")
    assert isinstance(issues_freshness, dict)
    assert isinstance(milestones_freshness, dict)

    if markdown_result.exit_code != expected_exit:
        return (
            "check_activation_triggers(markdown) gate_open/exit mismatch: "
            f"gate_open={gate_open!r} exit={markdown_result.exit_code}."
        )

    expected_lines = (
        f"- Mode: `{activation_payload['mode']}`",
        f"- Issues snapshot: `{inputs['issues_json']}`",
        f"- Milestones snapshot: `{inputs['milestones_json']}`",
        f"- Catalog JSON: `{inputs['catalog_json']}`",
        f"- Open blockers JSON: {open_blockers_input_text}",
        f"- T4 governance overlay JSON: {t4_overlay_input_text}",
        f"- Activation required: `{bool_text(activation_required)}`",
        f"- T4 new scope publish: `{bool_text(t4_new_scope_publish)}`",
        f"- T4 source: `{t4_source}`",
        f"- Gate open: `{bool_text(gate_open)}`",
        f"- Queue state: `{queue_state}`",
        f"- Exit code: `{expected_exit}`",
        f"- Open blockers count: `{open_blockers_count}`",
        f"- Open blockers trigger fired: `{bool_text(open_blockers_trigger_fired)}`",
        "## Snapshot Freshness",
        "| Snapshot | Requested | Max age (s) | Generated at UTC | Age (s) | Fresh |",
        "## Trigger Results",
        "| Trigger ID | Fired | Count | Condition |",
    )
    for expected_line in expected_lines:
        if expected_line in markdown_result.stdout:
            continue
        return (
            "check_activation_triggers(markdown) missing deterministic line "
            f"{expected_line!r}."
        )

    issues_freshness_error = validate_markdown_freshness_row(
        markdown_text=markdown_result.stdout,
        label="Issues",
        freshness_entry=issues_freshness,
    )
    if issues_freshness_error is not None:
        return issues_freshness_error

    milestones_freshness_error = validate_markdown_freshness_row(
        markdown_text=markdown_result.stdout,
        label="Milestones",
        freshness_entry=milestones_freshness,
    )
    if milestones_freshness_error is not None:
        return milestones_freshness_error

    active_trigger_ids = activation_payload["active_trigger_ids"]
    assert isinstance(active_trigger_ids, list)
    if active_trigger_ids:
        expected_active_line = "- Active triggers: " + ", ".join(
            f"`{trigger_id}`" for trigger_id in active_trigger_ids
        )
    else:
        expected_active_line = "- Active triggers: _none_"
    if expected_active_line not in markdown_result.stdout:
        return (
            "check_activation_triggers(markdown) missing deterministic gate line "
            f"{expected_active_line!r}."
        )

    return None
