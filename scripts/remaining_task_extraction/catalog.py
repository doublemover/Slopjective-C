"""Catalog loading, filtering, grouping, and dispatch capacity policy."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Callable, Sequence

from objc3c_tooling.paths import display_path
from objc3c_tooling.public_workflow_output import normalize_newlines

from remaining_task_extraction.model import (
    CAPACITY_STATUS_ORDER_INDEX,
    DEFAULT_STATUSES,
    GLOBAL_WIP_CAP,
    HARD_CAP_BREACH_LOAD_RATIO,
    LANE_ORDER_INDEX,
    LANE_WIP_CAPS,
    OVERLAP_ACTIVE_STATUS,
    STATUS_ORDER_INDEX,
    WARNING_LOAD_RATIO,
    LaneCapacityRow,
    OverlapConflictRow,
    TaskRow,
)


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

