"""Markdown table scanning and OPEN blocker row extraction."""

from __future__ import annotations

from open_blocker_extraction.markdown_text import (
    find_first_index,
    format_decode_failure_byte_range,
    format_row_error,
    is_delimiter_row,
    is_none_like,
    is_table_row_line,
    normalize_space,
    normalized_header,
    parse_due_date_utc,
    split_table_row,
    strip_wrapping_backticks,
)
from open_blocker_extraction.path_filter import (
    iter_markdown_files,
    normalize_exclude_pattern,
    normalize_exclude_patterns,
)
from open_blocker_extraction.row_extraction import (
    blocker_sort_key,
    extract_open_blockers,
    parse_open_blocker_rows_for_file,
)
from open_blocker_extraction.table_schema import resolve_table_schema

__all__ = [
    "blocker_sort_key",
    "extract_open_blockers",
    "find_first_index",
    "format_decode_failure_byte_range",
    "format_row_error",
    "is_delimiter_row",
    "is_none_like",
    "is_table_row_line",
    "iter_markdown_files",
    "normalize_exclude_pattern",
    "normalize_exclude_patterns",
    "normalize_space",
    "normalized_header",
    "parse_due_date_utc",
    "parse_open_blocker_rows_for_file",
    "resolve_table_schema",
    "split_table_row",
    "strip_wrapping_backticks",
]
