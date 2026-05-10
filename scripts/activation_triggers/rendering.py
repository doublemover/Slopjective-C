from __future__ import annotations

from typing import Any

from objc3c_tooling.reports import markdown_table


def bool_text(value: bool) -> str:
    return "true" if value else "false"


def freshness_cell(raw_value: object) -> str:
    if raw_value is None:
        return "_none_"
    if isinstance(raw_value, bool):
        return f"`{bool_text(raw_value)}`"
    return f"`{raw_value}`"


def render_markdown(payload: dict[str, Any]) -> str:
    inputs = payload["inputs"]
    t4_overlay = payload["t4_governance_overlay"]
    open_blockers = payload["open_blockers"]
    freshness = payload["freshness"]
    triggers = payload["triggers"]
    active_trigger_ids = payload["active_trigger_ids"]

    lines = [
        "# Activation Trigger Check",
        "",
        f"- Mode: `{payload['mode']}`",
        f"- Contract ID: `{payload['contract_id']}`",
        f"- Fail closed: `{bool_text(bool(payload['fail_closed']))}`",
        f"- Issues snapshot: `{inputs['issues_json']}`",
        f"- Milestones snapshot: `{inputs['milestones_json']}`",
        f"- Catalog JSON: `{inputs['catalog_json']}`",
        (
            f"- Open blockers JSON: `{inputs['open_blockers_json']}`"
            if inputs["open_blockers_json"] is not None
            else "- Open blockers JSON: _none_"
        ),
        (
            f"- T4 governance overlay JSON: `{inputs['t4_governance_overlay_json']}`"
            if inputs["t4_governance_overlay_json"] is not None
            else "- T4 governance overlay JSON: _none_"
        ),
        "- Actionable statuses: " + ", ".join(f"`{item}`" for item in payload["actionable_statuses"]),
        "- Trigger order: " + ", ".join(f"`{item}`" for item in payload["trigger_order"]),
        f"- Open blockers count: `{open_blockers['count']}`",
        f"- Open blockers trigger fired: `{bool_text(bool(open_blockers['trigger_fired']))}`",
        f"- Activation required: `{bool_text(bool(payload['activation_required']))}`",
        f"- T4 new scope publish: `{bool_text(bool(t4_overlay['new_scope_publish']))}`",
        f"- T4 source: `{t4_overlay['source']}`",
        f"- Gate open: `{bool_text(bool(payload['gate_open']))}`",
        f"- Queue state: `{payload['queue_state']}`",
        f"- Exit code: `{payload['exit_code']}`",
        "",
        "## Snapshot Freshness",
        "",
        *markdown_table(
            ["Snapshot", "Requested", "Max age (s)", "Generated at UTC", "Age (s)", "Fresh"],
            [
                [
                    "Issues",
                    freshness_cell(freshness["issues"]["requested"]),
                    freshness_cell(freshness["issues"]["max_age_seconds"]),
                    freshness_cell(freshness["issues"]["generated_at_utc"]),
                    freshness_cell(freshness["issues"]["age_seconds"]),
                    freshness_cell(freshness["issues"]["fresh"]),
                ],
                [
                    "Milestones",
                    freshness_cell(freshness["milestones"]["requested"]),
                    freshness_cell(freshness["milestones"]["max_age_seconds"]),
                    freshness_cell(freshness["milestones"]["generated_at_utc"]),
                    freshness_cell(freshness["milestones"]["age_seconds"]),
                    freshness_cell(freshness["milestones"]["fresh"]),
                ],
            ],
        ),
        "",
        "## Trigger Results",
        "",
        *markdown_table(
            ["Trigger ID", "Fired", "Count", "Condition"],
            [
                [
                    f"`{entry['id']}`",
                    f"`{bool_text(bool(entry['fired']))}`",
                    entry["count"],
                    entry["condition"],
                ]
                for entry in triggers
            ],
        ),
    ]
    lines.append("")
    if active_trigger_ids:
        lines.append("- Active triggers: " + ", ".join(f"`{item}`" for item in active_trigger_ids))
    else:
        lines.append("- Active triggers: _none_")
    return "\n".join(lines) + "\n"
