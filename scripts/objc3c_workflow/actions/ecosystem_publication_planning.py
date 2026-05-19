"""Planning-publication workflow actions."""

from __future__ import annotations

from .ecosystem_publication_contracts import (
    PLANNING_ISSUE_PUBLISHER_PY,
    PLANNING_PUBLICATION_AUDIT_PY,
)
from .ecosystem_publication_runner import run_publication_action


def action_publish_planning_issues(rest: list[str]) -> int:
    return run_publication_action("publish-planning-issues", rest)


def action_check_planning_publication_drift(rest: list[str]) -> int:
    return run_publication_action("check-planning-publication-drift", rest)


__all__ = [
    "PLANNING_ISSUE_PUBLISHER_PY",
    "PLANNING_PUBLICATION_AUDIT_PY",
    "action_check_planning_publication_drift",
    "action_publish_planning_issues",
]
