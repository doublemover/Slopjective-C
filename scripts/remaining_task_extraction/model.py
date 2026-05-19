"""Remaining-task backlog model and policy constants."""

from __future__ import annotations

from dataclasses import dataclass

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
