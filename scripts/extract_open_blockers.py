#!/usr/bin/env python3
"""Extract OPEN blocker rows from planning markdown tables."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Sequence

from objc3c_tooling.paths import display_path
from open_blocker_extraction.markdown_scan import (
    blocker_sort_key,
    extract_open_blockers,
    find_first_index,
    format_decode_failure_byte_range,
    format_row_error,
    is_delimiter_row,
    is_none_like,
    is_table_row_line,
    iter_markdown_files,
    normalize_exclude_pattern,
    normalize_exclude_patterns,
    normalize_space,
    normalized_header,
    parse_due_date_utc,
    parse_open_blocker_rows_for_file,
    resolve_table_schema,
    split_table_row,
    strip_wrapping_backticks,
)
from open_blocker_extraction.model import (
    BLOCKER_ID_RE,
    INLINE_BLOCKER_ID_RE,
    ISO_DATE_RE,
    ISO_UTC_SECOND_RE,
    TABLE_DELIMITER_CELL_RE,
    BlockerTableSchema,
    OpenBlocker,
)
from open_blocker_extraction.rendering import (
    blocker_to_dict,
    escape_markdown_cell,
    render_json,
    render_markdown,
    render_snapshot_json,
    snapshot_row_to_dict,
    validate_generated_at_utc,
    validate_snapshot_args,
    validate_snapshot_source,
)

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_ROOT = ROOT / "spec" / "planning"

__all__ = [
    "BLOCKER_ID_RE",
    "DEFAULT_ROOT",
    "INLINE_BLOCKER_ID_RE",
    "ISO_DATE_RE",
    "ISO_UTC_SECOND_RE",
    "ROOT",
    "TABLE_DELIMITER_CELL_RE",
    "BlockerTableSchema",
    "OpenBlocker",
    "blocker_sort_key",
    "blocker_to_dict",
    "build_parser",
    "escape_markdown_cell",
    "extract_open_blockers",
    "find_first_index",
    "format_decode_failure_byte_range",
    "format_row_error",
    "is_delimiter_row",
    "is_none_like",
    "is_table_row_line",
    "iter_markdown_files",
    "main",
    "normalize_exclude_pattern",
    "normalize_exclude_patterns",
    "normalize_space",
    "normalized_header",
    "parse_due_date_utc",
    "parse_open_blocker_rows_for_file",
    "render_json",
    "render_markdown",
    "render_snapshot_json",
    "resolve_root_path",
    "resolve_table_schema",
    "snapshot_row_to_dict",
    "split_table_row",
    "strip_wrapping_backticks",
    "validate_generated_at_utc",
    "validate_snapshot_args",
    "validate_snapshot_source",
    "write_stdout",
]


def write_stdout(value: str) -> None:
    buffer = getattr(sys.stdout, "buffer", None)
    if buffer is not None:
        buffer.write(value.encode("utf-8"))
        return
    sys.stdout.write(value)


def resolve_root_path(raw_root: Path) -> Path:
    if raw_root.is_absolute():
        return raw_root
    return ROOT / raw_root


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="extract_open_blockers.py",
        description=(
            "Extract deterministic OPEN blocker rows from markdown tables under a root directory."
        ),
    )
    parser.add_argument(
        "--root",
        type=Path,
        default=DEFAULT_ROOT,
        help=f"Root directory to scan recursively for markdown files (default: {display_path(DEFAULT_ROOT)}).",
    )
    parser.add_argument(
        "--format",
        choices=("json", "markdown", "snapshot-json"),
        default="json",
        help="Output format: json (default), markdown, or snapshot-json.",
    )
    parser.add_argument(
        "--generated-at-utc",
        help=(
            "Required with --format snapshot-json. Strict UTC timestamp "
            "(YYYY-MM-DDTHH:MM:SSZ)."
        ),
    )
    parser.add_argument(
        "--source",
        help=(
            "Required with --format snapshot-json. Canonical non-empty source string "
            "with no leading/trailing whitespace."
        ),
    )
    parser.add_argument(
        "--exclude-path",
        action="append",
        default=[],
        dest="exclude_paths",
        help=(
            "Repository-relative glob path to exclude from markdown scanning. "
            "Repeatable; default is none."
        ),
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        snapshot_meta = validate_snapshot_args(args)
        exclude_patterns = normalize_exclude_patterns(args.exclude_paths)
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    root = resolve_root_path(args.root)
    try:
        rows = extract_open_blockers(root, exclude_patterns=exclude_patterns)
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    if args.format == "markdown":
        write_stdout(render_markdown(rows))
    elif args.format == "snapshot-json":
        if snapshot_meta is None:
            print(
                "error: internal validation failure for snapshot-json mode",
                file=sys.stderr,
            )
            return 2
        generated_at_utc, source = snapshot_meta
        write_stdout(
            render_snapshot_json(
                rows,
                generated_at_utc=generated_at_utc,
                source=source,
            )
        )
    else:
        write_stdout(render_json(rows))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
