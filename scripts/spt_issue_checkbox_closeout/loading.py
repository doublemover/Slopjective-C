from __future__ import annotations

from pathlib import Path

from .checkbox_state import CatalogSourceState, collect_source_state
from .models import CatalogTask
from .paths import ROOT


def resolve_optional_root_path(path: Path | None) -> Path | None:
    if path is None:
        return None
    if path.is_absolute():
        return path
    return ROOT / path


def resolve_catalog_path(catalog_arg: Path | None, default_catalog: Path) -> Path:
    if catalog_arg is None:
        return default_catalog
    if catalog_arg.is_absolute():
        return catalog_arg
    return ROOT / catalog_arg


def filter_catalog_tasks(
    catalog_tasks: list[CatalogTask],
    *,
    lane: str | None,
    task_id_prefix: str,
) -> list[CatalogTask]:
    if lane:
        filtered = [task for task in catalog_tasks if f"[Lane {lane}]" in task.title]
    else:
        filtered = catalog_tasks

    return [task for task in filtered if task.task_id.startswith(task_id_prefix)]
