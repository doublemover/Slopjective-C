"""Deterministic sorting helpers for remaining-task catalog rows."""

from __future__ import annotations

from typing import Callable, Sequence

from remaining_task_extraction.model import (
    CAPACITY_STATUS_ORDER_INDEX,
    LANE_ORDER_INDEX,
    STATUS_ORDER_INDEX,
    TaskRow,
)


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


__all__ = [
    "capacity_status_sort_key",
    "dedupe_sort",
    "lane_sort_key",
    "row_sort_key",
    "status_sort_key",
    "text_sort_key",
]
