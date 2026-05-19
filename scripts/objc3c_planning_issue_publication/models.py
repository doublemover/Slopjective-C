"""Data models for planning issue publication orchestration."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from .contracts import LabelDefinition


@dataclass(frozen=True)
class PublicationInputs:
    payload_path: Path
    report_path: Path
    payload: dict[str, Any]
    existing_report: dict[str, Any]
    repo: str


@dataclass(frozen=True)
class IssuePublicationPlan:
    draft_id: str
    milestone_number: int
    relationship_section: str
    body: str
    labels: list[str]
    existing_number: int | None


@dataclass(frozen=True)
class DryRunPublicationResult:
    repo: str
    report: dict[str, Any]
    unresolved_dependencies: list[dict[str, Any]]


@dataclass(frozen=True)
class AppliedPublicationResult:
    repo: str
    report: dict[str, Any]
    labels: dict[str, LabelDefinition]
    label_summary: dict[str, Any]
    milestone_report: dict[str, Any]
    issue_report: dict[str, Any]
