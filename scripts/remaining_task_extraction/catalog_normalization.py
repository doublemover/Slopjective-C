"""Remaining-task catalog loading and field normalization."""

from __future__ import annotations

import json
from pathlib import Path

from objc3c_tooling.paths import display_path
from objc3c_tooling.public_workflow_output import normalize_newlines

from remaining_task_extraction.model import TaskRow


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


__all__ = [
    "load_catalog_rows",
    "normalize_catalog_path",
    "normalize_inline_text",
    "normalize_lane",
    "normalize_status",
    "parse_positive_line",
]
