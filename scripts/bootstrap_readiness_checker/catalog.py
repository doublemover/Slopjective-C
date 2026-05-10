from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path


def count_open_catalog_tasks(root: Any, *, path: Path) -> int:
    if not isinstance(root, dict):
        raise ValueError(
            f"error: catalog snapshot {display_path(path)} must be a JSON object"
        )
    tasks = root.get("tasks")
    if not isinstance(tasks, list):
        raise ValueError(
            f"error: catalog snapshot {display_path(path)} missing list field 'tasks'"
        )

    open_count = 0
    for index, task in enumerate(tasks):
        if not isinstance(task, dict):
            raise ValueError(
                f"error: catalog snapshot task row {index} in {display_path(path)} "
                "must be an object"
            )
        status = task.get("execution_status")
        if not isinstance(status, str) or not status.strip():
            raise ValueError(
                f"error: catalog snapshot task row {index} in {display_path(path)} "
                "missing non-empty 'execution_status'"
            )
        if status not in {"closed", "done"}:
            open_count += 1
    return open_count
