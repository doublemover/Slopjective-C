from __future__ import annotations

from typing import Any

from .catalog import validate_catalog_status_invariants
from .models import OverrideEntry
from .text import normalize_status


def apply_overrides(
    catalog: dict[str, Any],
    overrides: dict[str, OverrideEntry],
) -> tuple[list[dict[str, Any]], list[str]]:
    validate_catalog_status_invariants(
        catalog,
        source="catalog before overrides",
        allow_missing_task_ids=set(overrides.keys()),
    )

    tasks = catalog["tasks"]
    assert isinstance(tasks, list)

    touched_task_ids: set[str] = set()
    changes: list[dict[str, Any]] = []

    for raw_task in tasks:
        if not isinstance(raw_task, dict):
            continue
        task_id = raw_task.get("task_id")
        if not isinstance(task_id, str):
            continue

        entry = overrides.get(task_id)
        if entry is None:
            continue

        touched_task_ids.add(task_id)
        before_status_text = normalize_status(raw_task.get("execution_status"))
        raw_task["execution_status"] = entry.recommended_status
        raw_task["execution_status_rationale"] = entry.rationale
        raw_task["execution_status_evidence_refs"] = list(entry.evidence_refs)
        raw_task["execution_status_override_source"] = entry.source_path.as_posix()

        changes.append(
            {
                "task_id": task_id,
                "before_status": before_status_text or "(unset)",
                "after_status": entry.recommended_status,
                "override_source": entry.source_path.as_posix(),
            }
        )

    validate_catalog_status_invariants(catalog, source="catalog after overrides")

    missing = sorted(task_id for task_id in overrides if task_id not in touched_task_ids)
    return sorted(changes, key=lambda row: row["task_id"]), missing
