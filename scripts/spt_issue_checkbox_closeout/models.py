from __future__ import annotations

import re
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class CatalogTask:
    task_id: str
    path: Path
    line: int
    title: str
    task_key: str | None = None
    source_line_hash: str | None = None


@dataclass(frozen=True)
class IssueRef:
    number: int
    title: str
    url: str


@dataclass(frozen=True)
class CloseToolingConfig:
    catalog_default: Path
    task_id_prefix_default: str
    task_id_pattern: re.Pattern[str]


@dataclass(frozen=True)
class SourceLineResult:
    raw_line: str | None
    checked: bool
    display_line: str
    stale_reason: str | None


@dataclass(frozen=True)
class PlannedAction:
    task_id: str
    task_key: str | None
    issue_number: int
    issue_url: str
    source_path: str
    source_line: int
    source_checkbox_line: str


@dataclass(frozen=True)
class LoadedPlan:
    catalog_digest: str
    lane_filter: str | None
    task_id_prefix: str
    commit_sha: str | None
    actions: list[PlannedAction]
