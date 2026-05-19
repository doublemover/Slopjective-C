"""Markdown rendering for remaining-task backlog payloads."""

from __future__ import annotations

from typing import Sequence

from remaining_task_extraction.catalog import normalize_inline_text


def markdown_safe_cell(value: str) -> str:
    return normalize_inline_text(value).replace("|", "\\|")


def format_filter_values(values: Sequence[str]) -> str:
    if not values:
        return "_all lanes_"
    return ", ".join(f"`{value}`" for value in values)


def format_counts(entries: Sequence[dict[str, object]], *, key: str) -> str:
    if not entries:
        return "_none_"
    return ", ".join(f"`{entry[key]}`={entry['count']}" for entry in entries)


def format_ratio(value: object) -> str:
    if value is None:
        return "_n/a_"
    assert isinstance(value, float)
    return f"{value:.4f}"


def render_markdown(payload: dict[str, object]) -> str:
    summary = payload["summary"]
    assert isinstance(summary, dict)
    groups = payload["groups"]
    assert isinstance(groups, list)
    filters = payload["filters"]
    assert isinstance(filters, dict)
    capacity_rows = payload["capacity"]
    assert isinstance(capacity_rows, list)
    overlap_conflicts = payload["overlap_conflicts"]
    assert isinstance(overlap_conflicts, list)

    status_filters = filters.get("status", [])
    lane_filters = filters.get("lane", [])
    assert isinstance(status_filters, list)
    assert isinstance(lane_filters, list)

    lines: list[str] = [
        "# Remaining Task Backlog View",
        "",
        f"- Input: `{payload['input']}`",
        f"- Group by: `{payload['group_by']}`",
        f"- Status filters: {format_filter_values(status_filters)}",
        f"- Lane filters: {format_filter_values(lane_filters)}",
        "",
        "## Summary",
        "",
        f"- Total tasks: **{summary['total_tasks']}**",
        f"- Status counts: {format_counts(summary['status_counts'], key='status')}",
        f"- Lane counts: {format_counts(summary['lane_counts'], key='lane')}",
        f"- Path counts: {format_counts(summary['path_counts'], key='path')}",
        f"- Capacity status counts: {format_counts(summary['capacity_status_counts'], key='status')}",
        (
            "- Dispatch-intake: "
            f"`{summary['dispatch_intake']['status']}` "
            f"(`{summary['dispatch_intake']['recommendation']}`)"
        ),
        (
            "- Overlap conflicts: "
            f"{summary['overlap_conflicts']['count']} "
            f"(max allowed: {summary['overlap_conflicts']['max_allowed']}; "
            f"status=`{summary['overlap_conflicts']['status']}`; "
            f"intake recommendation: `{summary['overlap_conflicts']['intake_recommendation']}`)"
        ),
        "",
        "## Capacity Baseline",
        "",
        (
            "- Global capacity: "
            f"{summary['global_capacity']['active_issue_count']}/"
            f"{summary['global_capacity']['global_wip_cap']} "
            f"(load_ratio={format_ratio(summary['global_capacity']['load_ratio'])}, "
            f"status=`{summary['global_capacity']['status']}`)"
        ),
        (
            "- Global dispatch state: "
            f"`{summary['global_capacity']['dispatch_state']}` "
            f"(intake recommendation: "
            f"`{summary['global_capacity']['intake_recommendation']}`)"
        ),
        "",
        "| lane | active_issue_count | lane_wip_cap | load_ratio | status | dispatch_state | intake_recommendation | escalations |",
        "| --- | --- | --- | --- | --- | --- | --- | --- |",
    ]
    for row in capacity_rows:
        assert isinstance(row, dict)
        escalations_raw = row.get("escalations", [])
        assert isinstance(escalations_raw, list)
        escalation_text = ", ".join(f"`{entry}`" for entry in escalations_raw) if escalations_raw else "_none_"
        lines.append(
            "| "
            f"`{row['lane']}` | "
            f"{row['active_issue_count']} | "
            f"{row['lane_wip_cap'] if row['lane_wip_cap'] is not None else '_n/a_'} | "
            f"{format_ratio(row['load_ratio'])} | "
            f"`{row['status']}` | "
            f"`{row['dispatch_state']}` | "
            f"`{row['intake_recommendation']}` | "
            f"{escalation_text} |"
        )
    lines.append("")
    lines.append("## Overlap Conflicts")
    lines.append("")
    if not overlap_conflicts:
        lines.append("_none_")
        lines.append("")
    else:
        lines.append("| path | active_issue_count | lane_count | lanes | escalation |")
        lines.append("| --- | --- | --- | --- | --- |")
        for row in overlap_conflicts:
            assert isinstance(row, dict)
            lanes_raw = row.get("lanes", [])
            assert isinstance(lanes_raw, list)
            lanes_text = ", ".join(f"`{markdown_safe_cell(str(lane))}`" for lane in lanes_raw)
            lines.append(
                "| "
                f"`{markdown_safe_cell(str(row['path']))}` | "
                f"{row['active_issue_count']} | "
                f"{row['lane_count']} | "
                f"{lanes_text if lanes_text else '_none_'} | "
                f"`{markdown_safe_cell(str(row['escalation']))}` |"
            )
        lines.append("")

    if not groups:
        lines.extend(["_No matching tasks._", ""])
        return "\n".join(lines)

    for group in groups:
        assert isinstance(group, dict)
        group_value = markdown_safe_cell(str(group["group"]))
        count = group["count"]
        tasks = group["tasks"]
        assert isinstance(tasks, list)
        lines.append(f"## {payload['group_by']} `{group_value}` ({count} tasks)")
        lines.append("")
        lines.append("| task_id | title | lane | status | path | line |")
        lines.append("| --- | --- | --- | --- | --- | --- |")
        for task in tasks:
            assert isinstance(task, dict)
            lines.append(
                "| "
                f"`{markdown_safe_cell(str(task['task_id']))}` | "
                f"{markdown_safe_cell(str(task['title']))} | "
                f"`{markdown_safe_cell(str(task['lane']))}` | "
                f"`{markdown_safe_cell(str(task['status']))}` | "
                f"`{markdown_safe_cell(str(task['path']))}` | "
                f"{task['line']} |"
            )
        lines.append("")

    return "\n".join(lines).rstrip() + "\n"
