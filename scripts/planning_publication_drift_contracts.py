"""Planning publication drift report contracts."""

from __future__ import annotations

if __package__:
    from .planning_publication_drift_contracts_support import (
        DRIFT_CONTRACT_ID,
        JsonObject,
        PlanningPublicationDriftInputs,
        PlanningPublicationDriftPaths,
        append_failure,
        build_drift_report,
        compare_publication_references,
        compare_report_contract,
        issue_ref,
        publication_snapshot,
        render_markdown_report,
        repo_relative_path,
        success_status_line,
    )
else:
    from planning_publication_drift_contracts_support import (
        DRIFT_CONTRACT_ID,
        JsonObject,
        PlanningPublicationDriftInputs,
        PlanningPublicationDriftPaths,
        append_failure,
        build_drift_report,
        compare_publication_references,
        compare_report_contract,
        issue_ref,
        publication_snapshot,
        render_markdown_report,
        repo_relative_path,
        success_status_line,
    )


__all__ = [
    "DRIFT_CONTRACT_ID",
    "JsonObject",
    "PlanningPublicationDriftInputs",
    "PlanningPublicationDriftPaths",
    "append_failure",
    "build_drift_report",
    "compare_publication_references",
    "compare_report_contract",
    "issue_ref",
    "publication_snapshot",
    "render_markdown_report",
    "repo_relative_path",
    "success_status_line",
]
