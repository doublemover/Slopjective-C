#!/usr/bin/env python3
"""Extract OPEN blocker rows from planning markdown tables."""

from __future__ import annotations

import argparse
import json
import re
import sys
from fnmatch import fnmatchcase
from datetime import datetime
from dataclasses import dataclass
from pathlib import Path
from typing import Callable, Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_ROOT = ROOT / "spec" / "planning"

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


def write_stdout(value: str) -> None:
    buffer = getattr(sys.stdout, "buffer", None)
    if buffer is not None:
        buffer.write(value.encode("utf-8"))
        return
    sys.stdout.write(value)


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


def resolve_table_schema(header_cells: Sequence[str]) -> BlockerTableSchema | None:
    normalized_headers = [normalized_header(cell) for cell in header_cells]

    blocker_id_index = find_first_index(
        normalized_headers,
        lambda header: "blocker" in header and "id" in header,
    )
    if blocker_id_index is None:
        return None

    status_index = find_first_index(
        normalized_headers,
        lambda header: "status" in header or header == "state",
    )
    if status_index is None:
        return None

    owner_index = find_first_index(
        normalized_headers,
        lambda header: "responsible owner" in header,
    )
    if owner_index is None:
        owner_index = find_first_index(
            normalized_headers,
            lambda header: header == "owner",
        )
    if owner_index is None:
        owner_index = find_first_index(
            normalized_headers,
            lambda header: "owner" in header and "action" not in header,
        )

    due_date_index = find_first_index(
        normalized_headers,
        lambda header: "due date" in header,
    )

    excluded = {blocker_id_index, status_index}
    if owner_index is not None:
        excluded.add(owner_index)
    if due_date_index is not None:
        excluded.add(due_date_index)

    summary_index: int | None = None
    summary_priorities = (
        "blocking condition",
        "former blocking condition",
        "condition",
        "transition summary",
        "impacted scope",
        "blocks criteria",
        "blocks",
        "blocked",
        "summary",
    )
    for needle in summary_priorities:
        summary_index = find_first_index(
            normalized_headers,
            lambda header, needle=needle: needle in header,
        )
        if summary_index is not None and summary_index not in excluded:
            break
        summary_index = None
    if summary_index is None:
        for index in range(len(normalized_headers)):
            if index not in excluded:
                summary_index = index
                break

    owner_column_name = header_cells[owner_index] if owner_index is not None else None
    summary_column_name = header_cells[summary_index] if summary_index is not None else None
    return BlockerTableSchema(
        blocker_id_index=blocker_id_index,
        status_index=status_index,
        owner_index=owner_index,
        due_date_index=due_date_index,
        summary_index=summary_index,
        owner_column_name=owner_column_name,
        summary_column_name=summary_column_name,
    )


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


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def resolve_root_path(raw_root: Path) -> Path:
    if raw_root.is_absolute():
        return raw_root
    return ROOT / raw_root


def normalize_exclude_pattern(raw_pattern: str) -> str:
    if raw_pattern != raw_pattern.strip():
        raise ValueError(
            "invalid --exclude-path: value must not include leading or trailing whitespace"
        )
    normalized = raw_pattern.replace("\\", "/")
    if not normalized:
        raise ValueError("invalid --exclude-path: value must be a non-empty glob pattern")
    if Path(normalized).is_absolute():
        raise ValueError("invalid --exclude-path: value must be repository-relative")
    while normalized.startswith("./"):
        normalized = normalized[2:]
    if not normalized:
        raise ValueError("invalid --exclude-path: value must be a non-empty glob pattern")
    return normalized


def normalize_exclude_patterns(raw_patterns: Sequence[str]) -> tuple[str, ...]:
    normalized = {
        normalize_exclude_pattern(raw_pattern)
        for raw_pattern in raw_patterns
    }
    return tuple(sorted(normalized, key=lambda value: (value.casefold(), value)))


def iter_markdown_files(
    root: Path,
    *,
    exclude_patterns: Sequence[str],
) -> list[Path]:
    root_resolved = root.resolve()

    def is_excluded(path: Path) -> bool:
        if not exclude_patterns:
            return False

        candidate_paths = {display_path(path)}
        resolved = path.resolve()
        try:
            root_relative = resolved.relative_to(root_resolved).as_posix()
            candidate_paths.add(root_relative)
        except ValueError:
            pass
        return any(
            fnmatchcase(candidate_path, pattern)
            for candidate_path in candidate_paths
            for pattern in exclude_patterns
        )

    return sorted(
        (
            path
            for path in root.rglob("*.md")
            if path.is_file()
            and not is_excluded(path)
        ),
        key=lambda path: (display_path(path).casefold(), display_path(path)),
    )


def format_decode_failure_byte_range(exc: UnicodeDecodeError) -> str:
    if exc.end <= exc.start + 1:
        return f"{exc.start}"
    return f"{exc.start}-{exc.end - 1}"


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
                            owner = strip_wrapping_backticks(
                                row_cells[schema.owner_index]
                            )
                            if not owner:
                                owner_label = (
                                    normalize_space(schema.owner_column_name or "owner")
                                )
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
                            summary = strip_wrapping_backticks(
                                row_cells[schema.summary_index]
                            )
                            if not summary:
                                summary_label = normalize_space(
                                    schema.summary_column_name or "summary"
                                )
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

                            blockers.append(
                                OpenBlocker(
                                    blocker_id=blocker_id,
                                    source_path=source_path,
                                    line=row_line,
                                    owner=owner,
                                    due_date_utc=due_date_utc,
                                    summary=summary,
                                    status="OPEN",
                                )
                            )
            table_end += 1

        index = table_end

    return blockers


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


def blocker_to_dict(row: OpenBlocker) -> dict[str, object]:
    return {
        "blocker_id": row.blocker_id,
        "source_path": row.source_path,
        "line": row.line,
        "owner": row.owner,
        "due_date_utc": row.due_date_utc,
        "summary": row.summary,
        "status": row.status,
    }


def render_json(rows: Sequence[OpenBlocker]) -> str:
    payload = [blocker_to_dict(row) for row in rows]
    return json.dumps(payload, indent=2) + "\n"


def validate_generated_at_utc(raw_value: str) -> str:
    value = raw_value.strip()
    if value != raw_value:
        raise ValueError(
            "invalid --generated-at-utc: value must not include leading or trailing whitespace"
        )
    if not ISO_UTC_SECOND_RE.fullmatch(value):
        raise ValueError(
            "invalid --generated-at-utc: expected strict UTC timestamp like YYYY-MM-DDTHH:MM:SSZ"
        )
    try:
        datetime.strptime(value, "%Y-%m-%dT%H:%M:%SZ")
    except ValueError as exc:
        raise ValueError(
            "invalid --generated-at-utc: timestamp is not a valid UTC date-time"
        ) from exc
    return value


def validate_snapshot_source(raw_value: str) -> str:
    value = raw_value.strip()
    if not value:
        raise ValueError("invalid --source: value must be a non-empty canonical string")
    if value != raw_value:
        raise ValueError(
            "invalid --source: value must not include leading or trailing whitespace"
        )
    if normalize_space(value) != value:
        raise ValueError(
            "invalid --source: value must be canonical (no repeated internal whitespace)"
        )
    return value


def validate_snapshot_args(args: argparse.Namespace) -> tuple[str, str] | None:
    if args.format != "snapshot-json":
        if args.generated_at_utc is not None:
            raise ValueError(
                "--generated-at-utc is only supported when --format snapshot-json"
            )
        if args.source is not None:
            raise ValueError("--source is only supported when --format snapshot-json")
        return None

    if args.generated_at_utc is None:
        raise ValueError(
            "--generated-at-utc is required when --format snapshot-json"
        )
    if args.source is None:
        raise ValueError("--source is required when --format snapshot-json")

    return (
        validate_generated_at_utc(args.generated_at_utc),
        validate_snapshot_source(args.source),
    )


def snapshot_row_to_dict(row: OpenBlocker) -> dict[str, object]:
    line_number = row.line
    return {
        "blocker_id": row.blocker_id,
        "source_path": row.source_path,
        "line_number": line_number,
        "line": line_number,
    }


def render_snapshot_json(
    rows: Sequence[OpenBlocker],
    *,
    generated_at_utc: str,
    source: str,
) -> str:
    payload = {
        "generated_at_utc": generated_at_utc,
        "source": source,
        "open_blocker_count": len(rows),
        "open_blockers": [snapshot_row_to_dict(row) for row in rows],
    }
    return json.dumps(payload, indent=2) + "\n"


def escape_markdown_cell(value: str) -> str:
    return value.replace("|", r"\|")


def render_markdown(rows: Sequence[OpenBlocker]) -> str:
    lines = ["# Open blockers", ""]
    if not rows:
        lines.append("_No open blockers found._")
        return "\n".join(lines).rstrip() + "\n"

    lines.extend(
        [
            "| blocker_id | source_path | line | owner | due_date_utc | summary | status |",
            "| --- | --- | --- | --- | --- | --- | --- |",
        ]
    )
    for row in rows:
        due_value = f"`{row.due_date_utc}`" if row.due_date_utc is not None else "_none_"
        lines.append(
            "| "
            f"`{row.blocker_id}` | "
            f"`{row.source_path}` | "
            f"{row.line} | "
            f"{escape_markdown_cell(row.owner)} | "
            f"{due_value} | "
            f"{escape_markdown_cell(row.summary)} | "
            f"`{row.status}` |"
        )
    return "\n".join(lines).rstrip() + "\n"


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
