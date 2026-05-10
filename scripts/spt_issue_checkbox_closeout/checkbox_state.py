from __future__ import annotations

from dataclasses import dataclass

from .markdown import source_line_result
from .models import CatalogTask, SourceLineResult


@dataclass(frozen=True)
class CatalogSourceState:
    stale_entries: list[tuple[CatalogTask, str]]
    source_state_by_task_id: dict[str, SourceLineResult]
    task_by_task_id: dict[str, CatalogTask]


def collect_source_state(filtered: list[CatalogTask]) -> CatalogSourceState:
    stale_entries: list[tuple[CatalogTask, str]] = []
    source_state_by_task_id: dict[str, SourceLineResult] = {}
    task_by_task_id: dict[str, CatalogTask] = {}
    for task in filtered:
        source_state = source_line_result(task.path, task.line, task.source_line_hash)
        source_state_by_task_id[task.task_id] = source_state
        task_by_task_id[task.task_id] = task
        if source_state.stale_reason is not None:
            stale_entries.append((task, source_state.stale_reason))

    return CatalogSourceState(
        stale_entries=stale_entries,
        source_state_by_task_id=source_state_by_task_id,
        task_by_task_id=task_by_task_id,
    )
