from __future__ import annotations

from typing import Any


def normalize_text(value: str) -> str:
    return " ".join(value.strip().split())


def normalize_status(value: Any) -> str:
    if not isinstance(value, str):
        return ""
    return normalize_text(value).lower()


def task_row_label(raw_task: dict[str, Any], index: int) -> str:
    raw_task_id = raw_task.get("task_id")
    if isinstance(raw_task_id, str):
        task_id = normalize_text(raw_task_id)
        if task_id:
            return f"task_id '{task_id}'"
    return f"task at index {index}"
