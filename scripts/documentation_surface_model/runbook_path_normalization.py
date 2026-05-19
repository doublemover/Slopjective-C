"""Path normalization helpers for runbook documentation sources."""

from __future__ import annotations

from pathlib import Path

from .source_factory import ROOT


def normalize_runbook_source_path(path: Path, *, root: Path = ROOT) -> str:
    try:
        return path.relative_to(root).as_posix()
    except ValueError:
        return path.as_posix()


def runbook_source_path_sort_key(path: Path) -> tuple[str, ...]:
    return tuple(normalize_runbook_source_path(path).split("/"))


__all__ = (
    "normalize_runbook_source_path",
    "runbook_source_path_sort_key",
)
