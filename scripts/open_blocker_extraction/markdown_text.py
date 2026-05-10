"""Markdown text and table-row primitives for OPEN blocker extraction."""

from __future__ import annotations

from typing import Callable, Sequence

from open_blocker_extraction.model import (
    ISO_DATE_RE,
    ISO_UTC_SECOND_RE,
    TABLE_DELIMITER_CELL_RE,
)


def normalize_space(value: str) -> str:
    return " ".join(value.strip().split())


def strip_wrapping_backticks(value: str) -> str:
    normalized = normalize_space(value)
    if len(normalized) >= 2 and normalized.startswith("`") and normalized.endswith("`"):
        inner = normalized[1:-1].strip()
        if inner:
            return inner
    return normalized


def normalized_header(cell: str) -> str:
    return normalize_space(strip_wrapping_backticks(cell).lower())


def is_table_row_line(raw_line: str) -> bool:
    return raw_line.lstrip().startswith("|")


def split_table_row(raw_line: str) -> list[str]:
    stripped = raw_line.strip()
    if not stripped.startswith("|"):
        raise ValueError("not a markdown table row")
    return [cell.strip() for cell in stripped.strip("|").split("|")]


def is_delimiter_row(cells: Sequence[str]) -> bool:
    if not cells:
        return False
    for cell in cells:
        condensed = cell.replace(" ", "")
        if not TABLE_DELIMITER_CELL_RE.fullmatch(condensed):
            return False
    return True


def find_first_index(headers: Sequence[str], predicate: Callable[[str], bool]) -> int | None:
    for index, header in enumerate(headers):
        if predicate(header):
            return index
    return None


def is_none_like(value: str) -> bool:
    lowered = value.strip().lower()
    return lowered in {"", "-", "none", "n/a", "na", "_none_"}


def format_row_error(source_path: str, line: int, message: str) -> str:
    return f"{source_path}:{line}: {message}"


def parse_due_date_utc(
    raw_value: str,
    *,
    source_path: str,
    line: int,
) -> str | None:
    cleaned = strip_wrapping_backticks(raw_value)
    if is_none_like(cleaned):
        return None
    if ISO_DATE_RE.fullmatch(cleaned) or ISO_UTC_SECOND_RE.fullmatch(cleaned):
        return cleaned
    raise ValueError(
        format_row_error(
            source_path,
            line,
            "invalid due date value; expected YYYY-MM-DD or YYYY-MM-DDTHH:MM:SSZ",
        )
    )


def format_decode_failure_byte_range(exc: UnicodeDecodeError) -> str:
    if exc.end <= exc.start + 1:
        return f"{exc.start}"
    return f"{exc.start}-{exc.end - 1}"
