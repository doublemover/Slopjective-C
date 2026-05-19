"""Filtering and sorting helpers for open-issue sources and records."""

from __future__ import annotations

from collections.abc import Iterable
from pathlib import Path

from .models import OpenIssueRecord


def is_part_file(path: Path) -> bool:
    return path.is_file() and path.match("PART_*.md")


def iter_part_files(spec_dir: Path) -> list[Path]:
    return sorted(path for path in spec_dir.glob("PART_*.md") if is_part_file(path))


def record_sort_key(record: OpenIssueRecord) -> tuple[str, int, str]:
    return (record.file, record.line, record.heading)


def sort_records(records: Iterable[OpenIssueRecord]) -> list[OpenIssueRecord]:
    return sorted(records, key=record_sort_key)


__all__ = (
    "is_part_file",
    "iter_part_files",
    "record_sort_key",
    "sort_records",
)
