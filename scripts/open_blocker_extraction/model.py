"""Data model and shared constants for open blocker extraction."""

from __future__ import annotations

import re
from dataclasses import dataclass

BLOCKER_ID_RE = re.compile(r"^BLK-[A-Z0-9-]+$", re.IGNORECASE)
INLINE_BLOCKER_ID_RE = re.compile(r"\bBLK-[A-Z0-9-]+\b", re.IGNORECASE)
TABLE_DELIMITER_CELL_RE = re.compile(r"^:?-{3,}:?$")
ISO_DATE_RE = re.compile(r"^\d{4}-\d{2}-\d{2}$")
ISO_UTC_SECOND_RE = re.compile(r"^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$")


@dataclass(frozen=True)
class OpenBlocker:
    blocker_id: str
    source_path: str
    line: int
    owner: str
    due_date_utc: str | None
    summary: str
    status: str


@dataclass(frozen=True)
class BlockerTableSchema:
    blocker_id_index: int
    status_index: int
    owner_index: int | None
    due_date_index: int | None
    summary_index: int | None
    owner_column_name: str | None
    summary_column_name: str | None

