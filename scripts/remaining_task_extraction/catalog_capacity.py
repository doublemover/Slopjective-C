"""Dispatch capacity and overlap policy for remaining-task catalogs."""

from __future__ import annotations

from typing import Sequence

from remaining_task_extraction.catalog_sorting import dedupe_sort
from remaining_task_extraction.catalog_sorting import lane_sort_key
from remaining_task_extraction.catalog_sorting import text_sort_key
from remaining_task_extraction.model import (
    CAPACITY_STATUS_ORDER_INDEX,
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


__all__ = [
    "capacity_status_severity",
    "classify_capacity_status",
    "classify_threshold_status",
    "compute_capacity_rows",
    "compute_dispatch_intake_status",
    "compute_overlap_conflicts",
    "dispatch_state_for_status",
    "intake_recommendation_for_status",
    "ratio",
]
