"""End-to-end orchestration for open-issue extraction."""

from __future__ import annotations

from pathlib import Path

from .filtering import iter_part_files, sort_records
from .models import ExtractionResult, OpenIssueRecord, ParseIssue
from .parsing import parse_open_issue_sections, parse_section_items


def collect_open_issue_records(spec_dir: Path) -> tuple[list[OpenIssueRecord], list[ParseIssue]]:
    records: list[OpenIssueRecord] = []
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
                OpenIssueRecord(
                    file=file_display,
                    heading=section.heading,
                    line=section.line,
                    items=tuple(items),
                )
            )

    return sort_records(records), parse_issues


def collect_open_issues(spec_dir: Path) -> ExtractionResult:
    records, parse_issues = collect_open_issue_records(spec_dir)
    return ExtractionResult(
        records=[record.to_public_dict() for record in records],
        parse_issues=parse_issues,
    )


def extract_open_issues(spec_dir: Path) -> tuple[list[dict[str, object]], list[ParseIssue]]:
    result = collect_open_issues(spec_dir)
    return result.records, result.parse_issues


__all__ = (
    "collect_open_issue_records",
    "collect_open_issues",
    "extract_open_issues",
)
