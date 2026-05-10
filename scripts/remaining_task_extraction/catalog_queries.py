"""Filtering, grouping, and counting helpers for remaining-task catalog rows."""

from __future__ import annotations

from typing import Callable, Sequence

from remaining_task_extraction.catalog_sorting import dedupe_sort
from remaining_task_extraction.catalog_sorting import lane_sort_key
from remaining_task_extraction.catalog_sorting import row_sort_key
from remaining_task_extraction.catalog_sorting import status_sort_key
from remaining_task_extraction.catalog_sorting import text_sort_key
from remaining_task_extraction.model import DEFAULT_STATUSES, TaskRow


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
    rows: Sequence[object],
    *,
    key_name: str,
    key_fn: Callable[[object], str],
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


__all__ = [
    "build_groups",
    "count_by",
    "filter_rows",
    "group_value_sort_key",
    "normalize_lane_filter",
    "normalize_status_filter",
]
