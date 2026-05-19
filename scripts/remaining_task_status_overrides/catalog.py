from __future__ import annotations

from typing import Any

from .constants import ALLOWED_STATUSES
from .json_files import read_json
from .text import normalize_status, normalize_text, task_row_label


def load_catalog(path: Any) -> dict[str, Any]:
    payload = read_json(path)
    if not isinstance(payload, dict):
        raise ValueError(f"{path}: catalog JSON root must be an object")
    tasks = payload.get("tasks")
    if not isinstance(tasks, list):
        raise ValueError(f"{path}: catalog JSON must contain a 'tasks' array")
    return payload


def validate_catalog_status_invariants(
    catalog: dict[str, Any],
    *,
    source: str,
    allow_missing_task_ids: set[str] | None = None,
) -> None:
    tasks = catalog.get("tasks")
    if not isinstance(tasks, list):
        raise ValueError(f"{source}: catalog JSON must contain a 'tasks' array")

    allowed = ", ".join(sorted(ALLOWED_STATUSES))
    allowed_missing = allow_missing_task_ids or set()
    problems: list[str] = []

    for index, raw_task in enumerate(tasks):
        if not isinstance(raw_task, dict):
            problems.append(f"{source}: task at index {index} must be an object")
            continue

        label = task_row_label(raw_task, index)
        raw_task_id = raw_task.get("task_id")
        normalized_task_id = ""
        if isinstance(raw_task_id, str):
            normalized_task_id = normalize_text(raw_task_id)
            if normalized_task_id:
                raw_task["task_id"] = normalized_task_id
        normalized_status = normalize_status(raw_task.get("execution_status"))
        if not normalized_status:
            if normalized_task_id and normalized_task_id in allowed_missing:
                continue
            problems.append(
                f"{source}: {label} is missing required 'execution_status' "
                f"(allowed: {allowed})"
            )
            continue
        if normalized_status not in ALLOWED_STATUSES:
            problems.append(
                f"{source}: {label} has invalid 'execution_status' value "
                f"'{normalized_status}' (allowed: {allowed})"
            )
            continue
        raw_task["execution_status"] = normalized_status

    if not problems:
        return

    preview = "\n".join(f"- {problem}" for problem in problems[:10])
    if len(problems) > 10:
        preview = f"{preview}\n- ... plus {len(problems) - 10} more issue(s)"
    raise ValueError(f"catalog status invariants failed:\n{preview}")
