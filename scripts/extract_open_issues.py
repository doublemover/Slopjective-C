#!/usr/bin/env python3
"""Extract structured Open issues entries from spec/PART_*.md files."""

from __future__ import annotations

import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SPEC_DIR = ROOT / "spec"

OPEN_ISSUES_HEADING_RE = re.compile(
    r"^##\s+(?P<heading>.+?\bOpen issues\b.*?)(?:\s+\{#[^}]+\}\s*)?$",
    re.IGNORECASE,
)
LEVEL2_HEADING_RE = re.compile(r"^##\s+")
LIST_ITEM_RE = re.compile(r"^\s*(?:[-*+]|[0-9]+[.)])\s+(?P<text>.+?)\s*$")
NON_ALNUM_RE = re.compile(r"[^a-z0-9]+")


@dataclass(frozen=True)
class ParseIssue:
    file: str
    line: int
    message: str


@dataclass(frozen=True)
class OpenIssuesSection:
    heading: str
    line: int
    body: list[str]


def normalize_space(text: str) -> str:
    return " ".join(text.strip().split())


def is_none_statement(text: str) -> bool:
    normalized = NON_ALNUM_RE.sub(" ", text.lower()).strip()
    return normalized.startswith("none") or "no open issues" in normalized


def iter_part_files(spec_dir: Path) -> list[Path]:
    return sorted(path for path in spec_dir.glob("PART_*.md") if path.is_file())


def parse_open_issue_sections(lines: list[str]) -> list[OpenIssuesSection]:
    sections: list[OpenIssuesSection] = []
    idx = 0

    while idx < len(lines):
        match = OPEN_ISSUES_HEADING_RE.match(lines[idx])
        if not match:
            idx += 1
            continue

        heading = normalize_space(match.group("heading"))
        heading_line = idx + 1
        end = idx + 1
        while end < len(lines) and not LEVEL2_HEADING_RE.match(lines[end]):
            end += 1

        sections.append(
            OpenIssuesSection(
                heading=heading,
                line=heading_line,
                body=lines[idx + 1 : end],
            )
        )
        idx = end

    return sections


def parse_section_items(
    section: OpenIssuesSection,
    *,
    file_display: str,
) -> tuple[list[str], list[ParseIssue]]:
    list_items: list[tuple[int, str]] = []
    prose_lines: list[tuple[int, str]] = []
    active_list_item_idx: int | None = None

    first_body_line = section.line + 1
    for offset, raw_line in enumerate(section.body):
        line_number = first_body_line + offset
        stripped = raw_line.strip()
        if not stripped:
            continue

        list_match = LIST_ITEM_RE.match(raw_line)
        if list_match:
            text = normalize_space(list_match.group("text"))
            if text:
                list_items.append((line_number, text))
                active_list_item_idx = len(list_items) - 1
                continue

        if active_list_item_idx is not None and (raw_line.startswith("  ") or raw_line.startswith("\t")):
            start_line, current_text = list_items[active_list_item_idx]
            list_items[active_list_item_idx] = (
                start_line,
                normalize_space(f"{current_text} {stripped}"),
            )
            continue

        prose_lines.append((line_number, normalize_space(stripped)))
        active_list_item_idx = None

    issues: list[ParseIssue] = []
    none_list_items = [(line, text) for line, text in list_items if is_none_statement(text)]
    concrete_list_items = [(line, text) for line, text in list_items if not is_none_statement(text)]
    prose_text = normalize_space(" ".join(text for _, text in prose_lines))
    prose_is_none = bool(prose_text) and is_none_statement(prose_text)

    if none_list_items and concrete_list_items:
        issues.append(
            ParseIssue(
                file=file_display,
                line=none_list_items[0][0],
                message=f"'{section.heading}' mixes 'none' and concrete list items",
            )
        )

    if prose_lines:
        first_prose_line = prose_lines[0][0]
        if prose_is_none and concrete_list_items:
            issues.append(
                ParseIssue(
                    file=file_display,
                    line=first_prose_line,
                    message=f"'{section.heading}' mixes a prose 'none' marker with list items",
                )
            )
        elif not prose_is_none:
            issues.append(
                ParseIssue(
                    file=file_display,
                    line=first_prose_line,
                    message=f"'{section.heading}' contains non-list prose",
                )
            )

    extracted_items = [text for _, text in concrete_list_items]
    if prose_text and not prose_is_none:
        extracted_items.append(prose_text)
    if not section.body or (not extracted_items and not none_list_items and not prose_is_none):
        issues.append(
            ParseIssue(
                file=file_display,
                line=section.line,
                message=f"'{section.heading}' has no parseable items",
            )
        )

    return extracted_items, issues


def extract_open_issues(spec_dir: Path) -> tuple[list[dict[str, object]], list[ParseIssue]]:
    records: list[dict[str, object]] = []
    parse_issues: list[ParseIssue] = []

    for path in iter_part_files(spec_dir):
        file_display = path.relative_to(spec_dir.parent).as_posix()
        try:
            lines = path.read_text(encoding="utf-8").splitlines()
        except OSError as exc:
            parse_issues.append(
                ParseIssue(
                    file=file_display,
                    line=1,
                    message=f"unable to read file: {exc}",
                )
            )
            continue

        for section in parse_open_issue_sections(lines):
            items, issues = parse_section_items(section, file_display=file_display)
            parse_issues.extend(issues)
            records.append(
                {
                    "file": file_display,
                    "heading": section.heading,
                    "line": section.line,
                    "items": items,
                }
            )

    return records, parse_issues


def render_json(records: list[dict[str, object]]) -> str:
    return json.dumps(records, indent=2) + "\n"


def render_markdown(records: list[dict[str, object]]) -> str:
    lines = ["# Open issues", ""]
    if not records:
        lines.append("_No open-issues sections found._")
        return "\n".join(lines) + "\n"

    for record in records:
        lines.append(
            f"## {record['file']} - {record['heading']} (line {record['line']})"
        )
        items = record["items"]
        assert isinstance(items, list)
        if items:
            for item in items:
                lines.append(f"- {item}")
        else:
            lines.append("- None")
        lines.append("")

    return "\n".join(lines).rstrip() + "\n"


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Extract '## ... Open issues' sections from spec/PART_*.md."
    )
    parser.add_argument(
        "--format",
        choices=("json", "markdown"),
        default="json",
        help="Output format: json (default) or markdown.",
    )
    parser.add_argument(
        "--strict",
        action="store_true",
        help="Exit with status 1 if malformed open-issues sections are detected.",
    )
    parser.add_argument(
        "--spec-dir",
        type=Path,
        default=DEFAULT_SPEC_DIR,
        help=f"Directory containing PART_*.md files (default: {DEFAULT_SPEC_DIR}).",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    records, parse_issues = extract_open_issues(args.spec_dir)

    if args.format == "markdown":
        sys.stdout.write(render_markdown(records))
    else:
        sys.stdout.write(render_json(records))

    for issue in parse_issues:
        print(f"{issue.file}:{issue.line}: {issue.message}", file=sys.stderr)

    if args.strict and parse_issues:
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
