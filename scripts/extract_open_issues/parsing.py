"""Markdown parsing for open-issue sections."""

from __future__ import annotations

import re
from collections.abc import Sequence

from .models import OpenIssuesSection, ParseIssue

OPEN_ISSUES_HEADING_RE = re.compile(
    r"^##\s+(?P<heading>.+?\bOpen issues\b.*?)(?:\s+\{#[^}]+\}\s*)?$",
    re.IGNORECASE,
)
LEVEL2_HEADING_RE = re.compile(r"^##\s+")
LIST_ITEM_RE = re.compile(r"^\s*(?:[-*+]|[0-9]+[.)])\s+(?P<text>.+?)\s*$")
NON_ALNUM_RE = re.compile(r"[^a-z0-9]+")


def normalize_space(text: str) -> str:
    return " ".join(text.strip().split())


def is_none_statement(text: str) -> bool:
    normalized = NON_ALNUM_RE.sub(" ", text.lower()).strip()
    return normalized.startswith("none") or "no open issues" in normalized


def parse_open_issue_sections(lines: Sequence[str]) -> list[OpenIssuesSection]:
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
                body=list(lines[idx + 1 : end]),
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


__all__ = (
    "LEVEL2_HEADING_RE",
    "LIST_ITEM_RE",
    "NON_ALNUM_RE",
    "OPEN_ISSUES_HEADING_RE",
    "is_none_statement",
    "normalize_space",
    "parse_open_issue_sections",
    "parse_section_items",
)
