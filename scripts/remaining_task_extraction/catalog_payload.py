"""Payload construction for remaining-task catalog views."""

from __future__ import annotations

from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path

from remaining_task_extraction.catalog_capacity import classify_capacity_status
from remaining_task_extraction.catalog_capacity import classify_threshold_status
from remaining_task_extraction.catalog_capacity import compute_capacity_rows
from remaining_task_extraction.catalog_capacity import compute_dispatch_intake_status
from remaining_task_extraction.catalog_capacity import compute_overlap_conflicts
from remaining_task_extraction.catalog_capacity import dispatch_state_for_status
from remaining_task_extraction.catalog_capacity import intake_recommendation_for_status
from remaining_task_extraction.catalog_capacity import ratio
from remaining_task_extraction.catalog_queries import count_by
from remaining_task_extraction.catalog_sorting import capacity_status_sort_key
from remaining_task_extraction.catalog_sorting import lane_sort_key
from remaining_task_extraction.catalog_sorting import status_sort_key
from remaining_task_extraction.catalog_sorting import text_sort_key
from remaining_task_extraction.model import (
    DEFAULT_STATUSES,
    GLOBAL_WIP_CAP,
    HARD_CAP_BREACH_LOAD_RATIO,
    LANE_WIP_CAPS,
    OVERLAP_ACTIVE_STATUS,
    WARNING_LOAD_RATIO,
    LaneCapacityRow,
    OverlapConflictRow,
    TaskRow,
)


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


__all__ = [
    "build_payload",
    "capacity_row_to_dict",
    "overlap_conflict_row_to_dict",
    "row_to_dict",
]
