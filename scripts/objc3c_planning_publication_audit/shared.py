"""Shared imports and payload predicates for planning-publication drift audits."""

from __future__ import annotations

from typing import Any

from .errors import DriftAuditError

try:
    import publish_objc3c_planning_issues as publisher
    from planning_publication_drift_contracts import (
        PlanningPublicationDriftInputs,
        PlanningPublicationDriftPaths,
        append_failure,
        build_drift_report,
        compare_publication_references,
        compare_report_contract,
        publication_snapshot,
        render_markdown_report,
        repo_relative_path,
        success_status_line,
    )
except ModuleNotFoundError:
    from scripts import publish_objc3c_planning_issues as publisher
    from scripts.planning_publication_drift_contracts import (
        PlanningPublicationDriftInputs,
        PlanningPublicationDriftPaths,
        append_failure,
        build_drift_report,
        compare_publication_references,
        compare_report_contract,
        publication_snapshot,
        render_markdown_report,
        repo_relative_path,
        success_status_line,
    )


def require_dict(value: Any, name: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise DriftAuditError(f"{name} must be an object")
    return value


__all__ = [
    "PlanningPublicationDriftInputs",
    "PlanningPublicationDriftPaths",
    "append_failure",
    "build_drift_report",
    "compare_publication_references",
    "compare_report_contract",
    "publication_snapshot",
    "publisher",
    "render_markdown_report",
    "repo_relative_path",
    "require_dict",
    "success_status_line",
]
