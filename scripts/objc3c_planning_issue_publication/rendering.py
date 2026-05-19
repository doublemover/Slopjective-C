"""Issue rendering helpers for planning issue publication."""

from __future__ import annotations

from typing import Any

from .contracts import (
    apply_relationship_section,
    dependencies_by_issue,
    issue_labels,
    issue_number,
    milestone_by_title,
    render_relationship_section,
)
from .models import IssuePublicationPlan


def build_issue_publication_plan(
    issue: dict[str, Any],
    payload: dict[str, Any],
    issue_report: dict[str, Any],
    milestone_report: dict[str, Any],
    blocked_by: dict[str, list[dict[str, str]]],
    blocks: dict[str, list[dict[str, str]]],
) -> IssuePublicationPlan:
    issue_id = issue["id"]
    milestone = milestone_by_title(payload)[issue["milestoneTitle"]]
    milestone_number = milestone_report[milestone["id"]]["number"]
    relationship_section = render_relationship_section(issue_id, issue_report, blocked_by, blocks)
    return IssuePublicationPlan(
        draft_id=issue_id,
        milestone_number=milestone_number,
        relationship_section=relationship_section,
        body=apply_relationship_section(issue["body"], relationship_section),
        labels=issue_labels(issue, issue_id, payload),
        existing_number=issue_number(issue_report, issue_id),
    )


def relationship_maps(
    payload: dict[str, Any],
) -> tuple[dict[str, list[dict[str, str]]], dict[str, list[dict[str, str]]]]:
    return dependencies_by_issue(payload)
