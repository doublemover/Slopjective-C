"""OPEN blocker row extraction from markdown tables."""

from __future__ import annotations

from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path

from open_blocker_extraction.markdown_text import (
    format_decode_failure_byte_range,
    format_row_error,
    is_delimiter_row,
    is_table_row_line,
    normalize_space,
    parse_due_date_utc,
    split_table_row,
    strip_wrapping_backticks,
)
from open_blocker_extraction.model import (
    BLOCKER_ID_RE,
    INLINE_BLOCKER_ID_RE,
    BlockerTableSchema,
    OpenBlocker,
)
from open_blocker_extraction.path_filter import iter_markdown_files
from open_blocker_extraction.table_schema import resolve_table_schema


def parse_open_blocker_rows_for_file(path: Path) -> list[OpenBlocker]:
    source_path = display_path(path)
    try:
        lines = path.read_text(encoding="utf-8").splitlines()
    except UnicodeDecodeError as exc:
        byte_range = format_decode_failure_byte_range(exc)
        raise ValueError(
            f"markdown file is not valid UTF-8: {source_path} (decode failure at byte {byte_range})"
        ) from exc
    except OSError as exc:
        raise ValueError(f"unable to read markdown file {source_path}: {exc}") from exc

    blockers: list[OpenBlocker] = []
    index = 0

    while index < len(lines):
        if not is_table_row_line(lines[index]):
            index += 1
            continue

        if index + 1 >= len(lines) or not is_table_row_line(lines[index + 1]):
            index += 1
            continue

        header_cells = split_table_row(lines[index])
        delimiter_cells = split_table_row(lines[index + 1])
        if len(header_cells) != len(delimiter_cells) or not is_delimiter_row(delimiter_cells):
            index += 1
            continue

        schema = resolve_table_schema(header_cells)
        table_end = index + 2
        while table_end < len(lines) and is_table_row_line(lines[table_end]):
            raw_row = lines[table_end]
            row_line = table_end + 1
            row_cells = split_table_row(raw_row)
            if schema is not None:
                row_has_blocker_token = INLINE_BLOCKER_ID_RE.search(raw_row) is not None
                if row_has_blocker_token and len(row_cells) != len(header_cells):
                    raise ValueError(
                        format_row_error(
                            source_path,
                            row_line,
                            (
                                "malformed blocker row; expected "
                                f"{len(header_cells)} columns, found {len(row_cells)}"
                            ),
                        )
                    )

                if len(row_cells) == len(header_cells):
                    blocker_id_raw = strip_wrapping_backticks(
                        row_cells[schema.blocker_id_index]
                    )
                    if BLOCKER_ID_RE.fullmatch(blocker_id_raw):
                        blocker_id = blocker_id_raw.upper()
                        status_raw = strip_wrapping_backticks(
                            row_cells[schema.status_index]
                        )
                        if not status_raw:
                            raise ValueError(
                                format_row_error(
                                    source_path,
                                    row_line,
                                    "malformed blocker row; status value is empty",
                                )
                            )
                        status = status_raw.upper()
                        if status == "OPEN":
                            blockers.append(
                                build_open_blocker_from_row(
                                    row_cells=row_cells,
                                    blocker_id=blocker_id,
                                    source_path=source_path,
                                    row_line=row_line,
                                    schema=schema,
                                )
                            )
            table_end += 1

        index = table_end

    return blockers


def build_open_blocker_from_row(
    *,
    row_cells: Sequence[str],
    blocker_id: str,
    source_path: str,
    row_line: int,
    schema: BlockerTableSchema,
) -> OpenBlocker:
    if schema.owner_index is None:
        raise ValueError(
            format_row_error(
                source_path,
                row_line,
                (
                    "malformed OPEN blocker row; table is missing "
                    "an owner column"
                ),
            )
        )
    owner = strip_wrapping_backticks(row_cells[schema.owner_index])
    if not owner:
        owner_label = normalize_space(schema.owner_column_name or "owner")
        raise ValueError(
            format_row_error(
                source_path,
                row_line,
                (
                    "malformed OPEN blocker row; owner value is empty "
                    f"in column '{owner_label}'"
                ),
            )
        )
    if schema.summary_index is None:
        raise ValueError(
            format_row_error(
                source_path,
                row_line,
                (
                    "malformed OPEN blocker row; table is missing "
                    "a summary column"
                ),
            )
        )
    summary = strip_wrapping_backticks(row_cells[schema.summary_index])
    if not summary:
        summary_label = normalize_space(schema.summary_column_name or "summary")
        raise ValueError(
            format_row_error(
                source_path,
                row_line,
                (
                    "malformed OPEN blocker row; summary value is empty "
                    f"in column '{summary_label}'"
                ),
            )
        )
    due_date_utc: str | None = None
    if schema.due_date_index is not None:
        due_date_utc = parse_due_date_utc(
            row_cells[schema.due_date_index],
            source_path=source_path,
            line=row_line,
        )

    return OpenBlocker(
        blocker_id=blocker_id,
        source_path=source_path,
        line=row_line,
        owner=owner,
        due_date_utc=due_date_utc,
        summary=summary,
        status="OPEN",
    )


def blocker_sort_key(row: OpenBlocker) -> tuple[object, ...]:
    return (
        row.source_path.casefold(),
        row.source_path,
        row.line,
        row.blocker_id.casefold(),
        row.blocker_id,
    )


def extract_open_blockers(
    root: Path,
    *,
    exclude_patterns: Sequence[str] = (),
) -> list[OpenBlocker]:
    if not root.exists():
        raise ValueError(f"root path does not exist: {display_path(root)}")
    if not root.is_dir():
        raise ValueError(f"root path is not a directory: {display_path(root)}")

    blockers: list[OpenBlocker] = []
    for markdown_path in iter_markdown_files(root, exclude_patterns=exclude_patterns):
        blockers.extend(parse_open_blocker_rows_for_file(markdown_path))
    return sorted(blockers, key=blocker_sort_key)
