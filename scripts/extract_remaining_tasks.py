#!/usr/bin/env python3
"""Emit deterministic backlog views from remaining_task_review_catalog.json."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Callable, Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_INPUT = ROOT / "spec" / "planning" / "remaining_task_review_catalog.json"
DEFAULT_STATUSES: tuple[str, ...] = ("open", "open-blocked", "blocked")
STATUS_SORT_ORDER: tuple[str, ...] = ("open", "open-blocked", "blocked", "complete", "missing")
LANE_SORT_ORDER: tuple[str, ...] = ("A", "B", "C", "D")
CAPACITY_STATUS_SORT_ORDER: tuple[str, ...] = ("pass", "drift", "fail")
LANE_WIP_CAPS: dict[str, int] = {
    "A": 4,
    "B": 5,
    "C": 4,
    "D": 3,
}
GLOBAL_WIP_CAP = 16
WARNING_LOAD_RATIO = 0.85
HARD_CAP_BREACH_LOAD_RATIO = 1.00
OVERLAP_ACTIVE_STATUS = "open"
DEFAULT_MAX_OVERLAP_CONFLICTS = 0

STATUS_ORDER_INDEX = {value: index for index, value in enumerate(STATUS_SORT_ORDER)}
LANE_ORDER_INDEX = {value: index for index, value in enumerate(LANE_SORT_ORDER)}
CAPACITY_STATUS_ORDER_INDEX = {
    value: index for index, value in enumerate(CAPACITY_STATUS_SORT_ORDER)
}


@dataclass(frozen=True)
class TaskRow:
    task_id: str
    title: str
    lane: str
    status: str
    path: str
    line: int


@dataclass(frozen=True)
class LaneCapacityRow:
    lane: str
    active_issue_count: int
    lane_wip_cap: int | None
    load_ratio: float | None
    status: str
    dispatch_state: str
    intake_recommendation: str
    escalations: tuple[str, ...]


@dataclass(frozen=True)
class OverlapConflictRow:
    path: str
    active_issue_count: int
    lanes: tuple[str, ...]


def normalize_newlines(value: str) -> str:
    return value.replace("\r\n", "\n").replace("\r", "\n")


def normalize_inline_text(value: str) -> str:
    normalized = normalize_newlines(value).strip()
    if "\n" not in normalized:
        return normalized
    return " ".join(part.strip() for part in normalized.split("\n") if part.strip()).strip()


def normalize_lane(raw: object) -> str:
    if not isinstance(raw, str):
        raise ValueError("task row has invalid 'lane'; expected non-empty string")
    lane = raw.strip().upper()
    if not lane:
        raise ValueError("task row has invalid 'lane'; expected non-empty string")
    return lane


def normalize_status(
    raw: object,
    *,
    allow_missing_status: bool,
    task_id: str,
    index: int,
) -> str:
    if isinstance(raw, str):
        status = raw.strip().lower()
        if status:
            return status

    if allow_missing_status:
        return "missing"

    raise ValueError(
        "task at index "
        f"{index} ({task_id}) is missing required 'execution_status'; "
        "pass --allow-missing-status to treat missing values as 'missing'"
    )


def normalize_catalog_path(raw: object) -> str:
    if not isinstance(raw, str):
        raise ValueError("task row has invalid 'path'; expected non-empty string")
    normalized = raw.strip().replace("\\", "/")
    if not normalized:
        raise ValueError("task row has invalid 'path'; expected non-empty string")
    return normalized


def parse_positive_line(raw: object) -> int:
    if not isinstance(raw, int) or raw <= 0:
        raise ValueError("task row has invalid 'line'; expected positive integer")
    return raw


def resolve_input_path(raw_input: Path) -> Path:
    if raw_input.is_absolute():
        return raw_input
    return ROOT / raw_input


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def load_catalog_rows(path: Path, *, allow_missing_status: bool) -> list[TaskRow]:
    if not path.exists():
        raise ValueError(f"catalog JSON file does not exist: {display_path(path)}")

    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except OSError as exc:
        raise ValueError(f"unable to read catalog JSON {display_path(path)}: {exc}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(f"invalid JSON in {display_path(path)}: {exc}") from exc

    if not isinstance(payload, dict):
        raise ValueError("catalog JSON root must be an object")

    raw_tasks = payload.get("tasks")
    if not isinstance(raw_tasks, list):
        raise ValueError("catalog JSON missing 'tasks' array")

    rows: list[TaskRow] = []
    for index, raw in enumerate(raw_tasks):
        if not isinstance(raw, dict):
            raise ValueError(f"task at index {index} must be an object")

        raw_task_id = raw.get("task_id")
        raw_title = raw.get("title")
        task_id = normalize_inline_text(raw_task_id) if isinstance(raw_task_id, str) else ""
        if not task_id:
            raise ValueError(f"task at index {index} has invalid 'task_id'; expected non-empty string")

        if isinstance(raw_title, str):
            title = normalize_inline_text(raw_title)
        else:
            title = task_id
        if not title:
            title = task_id

        rows.append(
            TaskRow(
                task_id=task_id,
                title=title,
                lane=normalize_lane(raw.get("lane")),
                status=normalize_status(
                    raw.get("execution_status"),
                    allow_missing_status=allow_missing_status,
                    task_id=task_id,
                    index=index,
                ),
                path=normalize_catalog_path(raw.get("path")),
                line=parse_positive_line(raw.get("line")),
            )
        )

    return rows


def status_sort_key(value: str) -> tuple[int, str, str]:
    return (STATUS_ORDER_INDEX.get(value, len(STATUS_ORDER_INDEX)), value.casefold(), value)


def lane_sort_key(value: str) -> tuple[int, str, str]:
    return (LANE_ORDER_INDEX.get(value, len(LANE_ORDER_INDEX)), value.casefold(), value)


def capacity_status_sort_key(value: str) -> tuple[int, str, str]:
    return (
        CAPACITY_STATUS_ORDER_INDEX.get(value, len(CAPACITY_STATUS_ORDER_INDEX)),
        value.casefold(),
        value,
    )


def text_sort_key(value: str) -> tuple[str, str]:
    return (value.casefold(), value)


def dedupe_sort(values: Sequence[str], *, sort_key: Callable[[str], tuple[object, ...]]) -> tuple[str, ...]:
    seen: set[str] = set()
    unique: list[str] = []
    for value in values:
        if value in seen:
            continue
        seen.add(value)
        unique.append(value)
    return tuple(sorted(unique, key=sort_key))


def row_sort_key(row: TaskRow) -> tuple[object, ...]:
    return (
        row.path.casefold(),
        row.path,
        row.line,
        row.task_id.casefold(),
        row.task_id,
        row.title.casefold(),
        row.title,
        row.lane.casefold(),
        row.lane,
        row.status.casefold(),
        row.status,
    )


def normalize_status_filter(raw_statuses: Sequence[str] | None) -> tuple[str, ...]:
    if not raw_statuses:
        return DEFAULT_STATUSES
    normalized: list[str] = []
    for raw in raw_statuses:
        status = raw.strip().lower()
        if not status:
            raise ValueError("status filters must be non-empty")
        normalized.append(status)
    return dedupe_sort(normalized, sort_key=status_sort_key)


def normalize_lane_filter(raw_lanes: Sequence[str] | None) -> tuple[str, ...]:
    if not raw_lanes:
        return ()
    normalized: list[str] = []
    for raw in raw_lanes:
        lane = raw.strip().upper()
        if not lane:
            raise ValueError("lane filters must be non-empty")
        normalized.append(lane)
    return dedupe_sort(normalized, sort_key=lane_sort_key)


def filter_rows(
    rows: Sequence[TaskRow],
    *,
    statuses: Sequence[str],
    lanes: Sequence[str],
) -> list[TaskRow]:
    status_set = set(statuses)
    lane_set = set(lanes)
    if lane_set:
        return [row for row in rows if row.status in status_set and row.lane in lane_set]
    return [row for row in rows if row.status in status_set]


def group_value_sort_key(group_by: str, value: str) -> tuple[object, ...]:
    if group_by == "lane":
        return lane_sort_key(value)
    if group_by == "status":
        return status_sort_key(value)
    return text_sort_key(value)


def build_groups(rows: Sequence[TaskRow], *, group_by: str) -> list[tuple[str, list[TaskRow]]]:
    grouped: dict[str, list[TaskRow]] = {}
    for row in rows:
        key = getattr(row, group_by)
        grouped.setdefault(key, []).append(row)

    groups: list[tuple[str, list[TaskRow]]] = []
    for key in sorted(grouped.keys(), key=lambda value: group_value_sort_key(group_by, value)):
        groups.append((key, sorted(grouped[key], key=row_sort_key)))
    return groups


def count_by(
    rows: Sequence[TaskRow],
    *,
    key_name: str,
    key_fn: Callable[[TaskRow], str],
    sort_key: Callable[[str], tuple[object, ...]],
) -> list[dict[str, object]]:
    counts: dict[str, int] = {}
    for row in rows:
        key = key_fn(row)
        counts[key] = counts.get(key, 0) + 1
    return [
        {key_name: key, "count": counts[key]}
        for key in sorted(counts.keys(), key=sort_key)
    ]


def ratio(numerator: int, denominator: int) -> float:
    return round(numerator / denominator, 4)


def classify_capacity_status(load_ratio: float) -> str:
    if load_ratio > HARD_CAP_BREACH_LOAD_RATIO:
        return "fail"
    if load_ratio > WARNING_LOAD_RATIO:
        return "drift"
    return "pass"


def intake_recommendation_for_status(status: str) -> str:
    if status == "pass":
        return "go"
    if status == "drift":
        return "hold"
    return "no-go"


def dispatch_state_for_status(status: str, *, active_issue_count: int, lane_wip_cap: int | None) -> str:
    if status == "pass":
        return "dispatch-open"
    if status == "drift":
        if lane_wip_cap is not None and active_issue_count >= lane_wip_cap:
            return "dispatch-p0-only-with-lane-d-approval"
        return "dispatch-warning"
    if lane_wip_cap is None:
        return "dispatch-frozen-unknown-lane"
    return "dispatch-frozen"


def capacity_status_severity(status: str) -> int:
    return CAPACITY_STATUS_ORDER_INDEX.get(status, len(CAPACITY_STATUS_ORDER_INDEX))


def classify_threshold_status(value: int, *, max_allowed: int) -> str:
    if value == 0:
        return "pass"
    if value <= max_allowed:
        return "drift"
    return "fail"


def compute_capacity_rows(rows: Sequence[TaskRow]) -> list[LaneCapacityRow]:
    active_counts: dict[str, int] = {}
    for row in rows:
        if row.status not in DEFAULT_STATUSES:
            continue
        active_counts[row.lane] = active_counts.get(row.lane, 0) + 1

    lanes = set(active_counts.keys())
    lanes.update(LANE_WIP_CAPS.keys())
    lane_order = sorted(lanes, key=lane_sort_key)

    capacity_rows: list[LaneCapacityRow] = []
    for lane in lane_order:
        active_issue_count = active_counts.get(lane, 0)
        lane_wip_cap = LANE_WIP_CAPS.get(lane)
        if lane_wip_cap is None:
            load_ratio: float | None = None
            status = "fail"
            escalations = ("ESC-CAP-02",)
        else:
            load_ratio = ratio(active_issue_count, lane_wip_cap)
            status = classify_capacity_status(load_ratio)
            if status == "pass":
                escalations = ()
            elif status == "drift":
                escalations = ("ESC-CAP-01",)
            else:
                escalations = ("ESC-CAP-02",)

        capacity_rows.append(
            LaneCapacityRow(
                lane=lane,
                active_issue_count=active_issue_count,
                lane_wip_cap=lane_wip_cap,
                load_ratio=load_ratio,
                status=status,
                dispatch_state=dispatch_state_for_status(
                    status,
                    active_issue_count=active_issue_count,
                    lane_wip_cap=lane_wip_cap,
                ),
                intake_recommendation=intake_recommendation_for_status(status),
                escalations=escalations,
            )
        )
    return capacity_rows


def compute_overlap_conflicts(rows: Sequence[TaskRow]) -> list[OverlapConflictRow]:
    active_lanes_by_path: dict[str, set[str]] = {}
    active_count_by_path: dict[str, int] = {}
    for row in rows:
        if row.status != OVERLAP_ACTIVE_STATUS:
            continue
        active_lanes_by_path.setdefault(row.path, set()).add(row.lane)
        active_count_by_path[row.path] = active_count_by_path.get(row.path, 0) + 1

    overlap_rows: list[OverlapConflictRow] = []
    for path in sorted(active_lanes_by_path.keys(), key=text_sort_key):
        lanes = dedupe_sort(tuple(active_lanes_by_path[path]), sort_key=lane_sort_key)
        if len(lanes) <= 1:
            continue
        overlap_rows.append(
            OverlapConflictRow(
                path=path,
                active_issue_count=active_count_by_path[path],
                lanes=lanes,
            )
        )
    return overlap_rows


def compute_dispatch_intake_status(
    capacity_rows: Sequence[LaneCapacityRow],
    *,
    global_status: str,
    overlap_status: str,
) -> tuple[str, str]:
    statuses = [row.status for row in capacity_rows]
    statuses.extend((global_status, overlap_status))
    if not statuses:
        return "pass", intake_recommendation_for_status("pass")

    worst_status = max(statuses, key=capacity_status_severity)
    return worst_status, intake_recommendation_for_status(worst_status)


def row_to_dict(row: TaskRow) -> dict[str, object]:
    return {
        "task_id": row.task_id,
        "title": row.title,
        "lane": row.lane,
        "status": row.status,
        "path": row.path,
        "line": row.line,
    }


def capacity_row_to_dict(row: LaneCapacityRow) -> dict[str, object]:
    payload: dict[str, object] = {
        "lane": row.lane,
        "active_issue_count": row.active_issue_count,
        "lane_wip_cap": row.lane_wip_cap,
        "status": row.status,
        "dispatch_state": row.dispatch_state,
        "intake_recommendation": row.intake_recommendation,
        "escalations": list(row.escalations),
    }
    payload["load_ratio"] = row.load_ratio
    return payload


def overlap_conflict_row_to_dict(row: OverlapConflictRow) -> dict[str, object]:
    return {
        "path": row.path,
        "active_issue_count": row.active_issue_count,
        "lane_count": len(row.lanes),
        "lanes": list(row.lanes),
        "escalation": "ESC-MERGE-01",
    }


def build_payload(
    *,
    input_path: Path,
    all_rows: Sequence[TaskRow],
    statuses: Sequence[str],
    lanes: Sequence[str],
    group_by: str,
    groups: Sequence[tuple[str, list[TaskRow]]],
    max_overlap_conflicts: int,
) -> dict[str, object]:
    flattened = [row for _, rows in groups for row in rows]
    capacity_rows = compute_capacity_rows(rows=all_rows)
    global_active_issue_count = sum(1 for row in all_rows if row.status in DEFAULT_STATUSES)
    global_load_ratio = ratio(global_active_issue_count, GLOBAL_WIP_CAP)
    global_status = classify_capacity_status(global_load_ratio)
    overlap_conflicts = compute_overlap_conflicts(rows=all_rows)
    overlap_conflict_count = len(overlap_conflicts)
    overlap_status = classify_threshold_status(
        overlap_conflict_count,
        max_allowed=max_overlap_conflicts,
    )
    dispatch_intake_status, dispatch_intake_recommendation = compute_dispatch_intake_status(
        capacity_rows,
        global_status=global_status,
        overlap_status=overlap_status,
    )
    summary = {
        "total_tasks": len(flattened),
        "status_counts": count_by(
            flattened,
            key_name="status",
            key_fn=lambda row: row.status,
            sort_key=status_sort_key,
        ),
        "lane_counts": count_by(
            flattened,
            key_name="lane",
            key_fn=lambda row: row.lane,
            sort_key=lane_sort_key,
        ),
        "path_counts": count_by(
            flattened,
            key_name="path",
            key_fn=lambda row: row.path,
            sort_key=text_sort_key,
        ),
        "capacity_status_counts": count_by(
            capacity_rows,
            key_name="status",
            key_fn=lambda row: row.status,
            sort_key=capacity_status_sort_key,
        ),
        "dispatch_intake": {
            "status": dispatch_intake_status,
            "recommendation": dispatch_intake_recommendation,
        },
        "overlap_conflicts": {
            "count": overlap_conflict_count,
            "max_allowed": max_overlap_conflicts,
            "status": overlap_status,
            "intake_recommendation": intake_recommendation_for_status(overlap_status),
        },
        "global_capacity": {
            "active_issue_count": global_active_issue_count,
            "global_wip_cap": GLOBAL_WIP_CAP,
            "load_ratio": global_load_ratio,
            "status": global_status,
            "dispatch_state": dispatch_state_for_status(
                global_status,
                active_issue_count=global_active_issue_count,
                lane_wip_cap=GLOBAL_WIP_CAP,
            ),
            "intake_recommendation": intake_recommendation_for_status(global_status),
        },
    }

    return {
        "input": display_path(input_path),
        "group_by": group_by,
        "filters": {
            "status": list(statuses),
            "lane": list(lanes),
        },
        "capacity_policy": {
            "lane_wip_caps": dict(LANE_WIP_CAPS),
            "global_wip_cap": GLOBAL_WIP_CAP,
            "warning_load_ratio_gt": WARNING_LOAD_RATIO,
            "hard_cap_breach_load_ratio_gt": HARD_CAP_BREACH_LOAD_RATIO,
            "active_issue_statuses": list(DEFAULT_STATUSES),
            "overlap_active_issue_statuses": [OVERLAP_ACTIVE_STATUS],
            "max_overlap_conflicts": max_overlap_conflicts,
        },
        "summary": summary,
        "capacity": [capacity_row_to_dict(row) for row in capacity_rows],
        "overlap_conflicts": [overlap_conflict_row_to_dict(row) for row in overlap_conflicts],
        "groups": [
            {
                "group": key,
                "count": len(rows),
                "tasks": [row_to_dict(row) for row in rows],
            }
            for key, rows in groups
        ],
        "tasks": [row_to_dict(row) for row in flattened],
    }


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


def render_json(payload: dict[str, object]) -> str:
    return json.dumps(payload, indent=2) + "\n"


def write_stdout(value: str) -> None:
    buffer = getattr(sys.stdout, "buffer", None)
    if buffer is not None:
        buffer.write(value.encode("utf-8"))
        return
    sys.stdout.write(value)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="extract_remaining_tasks.py",
        description=(
            "Read remaining_task_review_catalog.json and emit deterministic backlog views."
        ),
    )
    parser.add_argument(
        "--input",
        type=Path,
        default=DEFAULT_INPUT,
        help=(
            "Path to remaining_task_review_catalog.json "
            f"(default: {display_path(DEFAULT_INPUT)})."
        ),
    )
    parser.add_argument(
        "--format",
        choices=("json", "markdown"),
        default="json",
        help="Output format: json (default) or markdown.",
    )
    parser.add_argument(
        "--status",
        action="append",
        dest="status_filters",
        help=(
            "Repeatable execution status filter. "
            "Defaults to: open, open-blocked, blocked."
        ),
    )
    parser.add_argument(
        "--lane",
        action="append",
        dest="lane_filters",
        help="Repeatable lane filter (for example: A, B, C, D).",
    )
    parser.add_argument(
        "--group-by",
        choices=("lane", "status", "path"),
        default="lane",
        help="Group tasks by lane (default), status, or path.",
    )
    parser.add_argument(
        "--allow-missing-status",
        action="store_true",
        help=(
            "Allow missing/blank execution_status values and normalize them to "
            "'missing'."
        ),
    )
    parser.add_argument(
        "--max-dispatch-intake-status",
        choices=("pass", "drift", "fail"),
        help=(
            "Enforcement guard: exit non-zero when computed dispatch-intake status "
            "is more severe than this value."
        ),
    )
    parser.add_argument(
        "--max-overlap-conflicts",
        type=int,
        default=DEFAULT_MAX_OVERLAP_CONFLICTS,
        help=(
            "Classify overlap conflict severity using this threshold: "
            "0 conflicts=pass, 1..N conflicts=drift, >N conflicts=fail."
        ),
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        if args.max_overlap_conflicts < 0:
            raise ValueError("--max-overlap-conflicts must be >= 0")
        status_filters = normalize_status_filter(args.status_filters)
        lane_filters = normalize_lane_filter(args.lane_filters)
        input_path = resolve_input_path(args.input)
        rows = load_catalog_rows(
            input_path,
            allow_missing_status=args.allow_missing_status,
        )
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    filtered = filter_rows(rows, statuses=status_filters, lanes=lane_filters)
    groups = build_groups(filtered, group_by=args.group_by)
    payload = build_payload(
        input_path=input_path,
        all_rows=rows,
        statuses=status_filters,
        lanes=lane_filters,
        group_by=args.group_by,
        groups=groups,
        max_overlap_conflicts=args.max_overlap_conflicts,
    )

    if args.format == "markdown":
        write_stdout(render_markdown(payload))
    else:
        write_stdout(render_json(payload))

    if args.max_dispatch_intake_status is not None:
        summary = payload["summary"]
        assert isinstance(summary, dict)
        dispatch_intake = summary["dispatch_intake"]
        assert isinstance(dispatch_intake, dict)
        status = dispatch_intake["status"]
        assert isinstance(status, str)
        if capacity_status_severity(status) > capacity_status_severity(args.max_dispatch_intake_status):
            print(
                (
                    "error: dispatch-intake status exceeds threshold; "
                    f"status={status}, max_allowed={args.max_dispatch_intake_status}"
                ),
                file=sys.stderr,
            )
            return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
