"""Data models for open-issue extraction."""

from __future__ import annotations

from dataclasses import dataclass


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


@dataclass(frozen=True)
class OpenIssueRecord:
    file: str
    heading: str
    line: int
    items: tuple[str, ...]

    def to_public_dict(self) -> dict[str, object]:
        return {
            "file": self.file,
            "heading": self.heading,
            "line": self.line,
            "items": list(self.items),
        }


@dataclass(frozen=True)
class ExtractionResult:
    records: list[dict[str, object]]
    parse_issues: list[ParseIssue]


__all__ = (
    "ExtractionResult",
    "OpenIssueRecord",
    "OpenIssuesSection",
    "ParseIssue",
)
