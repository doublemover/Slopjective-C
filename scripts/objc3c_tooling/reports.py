"""Small Markdown and report-output primitives for repository tooling."""

from __future__ import annotations

from collections.abc import Iterable, Sequence
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import canonical_json, write_json_file, write_text_file
from objc3c_tooling.paths import display_path


def markdown_join(lines: Iterable[str]) -> str:
    return "\n".join(lines)


def markdown_heading(title: str, *, level: int = 1) -> str:
    if level < 1:
        raise ValueError("Markdown heading level must be at least 1")
    return f"{'#' * level} {title}"


def markdown_code(value: object) -> str:
    return f"`{value}`"


def markdown_bool(value: bool) -> str:
    return "true" if value else "false"


def markdown_optional_code(value: object | None, *, none_text: str = "_none_") -> str:
    return none_text if value is None else markdown_code(value)


def markdown_bullet(label: str, value: object | None, *, code: bool = True) -> str:
    if value is None:
        rendered = "_none_"
    else:
        rendered = markdown_code(value) if code else str(value)
    return f"- {label}: {rendered}"


def escape_markdown_table_cell(value: object | None) -> str:
    if value is None:
        return ""
    text = str(value).replace("\\", "\\\\")
    return text.replace("|", "\\|").replace("\r", " ").replace("\n", "<br>")


def markdown_table(headers: Sequence[object], rows: Iterable[Sequence[object | None]]) -> list[str]:
    column_count = len(headers)
    header = "| " + " | ".join(escape_markdown_table_cell(item) for item in headers) + " |"
    divider = "| " + " | ".join("---" for _ in headers) + " |"
    body: list[str] = []
    for row in rows:
        if len(row) != column_count:
            raise ValueError(f"Markdown table row has {len(row)} cells; expected {column_count}")
        body.append("| " + " | ".join(escape_markdown_table_cell(item) for item in row) + " |")
    return [header, divider, *body]


def artifact_path(path: Path | str) -> str:
    return display_path(path)


def write_report_outputs(
    *,
    summary: Any,
    json_path: Path | str,
    markdown_path: Path | str,
    markdown: str,
    sort_keys: bool = True,
) -> None:
    write_json_file(json_path, summary, sort_keys=sort_keys)
    write_text_file(markdown_path, markdown)


def expected_json_report(summary: Any, *, sort_keys: bool = True) -> str:
    return canonical_json(summary, sort_keys=sort_keys)
