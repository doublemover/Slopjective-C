from __future__ import annotations

from typing import Any, Mapping

from .constants import DRIFT_CONTRACT_ID
from .models import JsonObject, PlanningPublicationDriftInputs


def build_drift_report(inputs: PlanningPublicationDriftInputs) -> JsonObject:
    payload = inputs.payload
    report = inputs.report
    return {
        "contract_id": DRIFT_CONTRACT_ID,
        "repository": payload["repository"],
        "source_updated_at_utc": report["updatedAtUtc"],
        "payload_path": inputs.paths.payload_path,
        "publication_report_path": inputs.paths.publication_report_path,
        "markdown_report_path": inputs.paths.markdown_report_path,
        "milestone_count": len(payload["milestones"]),
        "issue_count": len(payload["issues"]),
        "dependency_count": len(payload.get("dependencies", [])),
        "live": inputs.live_summary,
        "failure_count": len(inputs.failures),
        "failures": inputs.failures,
    }


def success_status_line(
    payload: Mapping[str, Any],
    live_summary: Mapping[str, Any],
) -> str:
    return (
        "planning-publication-drift-audit: OK "
        f"milestones={len(payload['milestones'])} issues={len(payload['issues'])} "
        f"dependencies={len(payload.get('dependencies', []))} "
        f"live_issues={live_summary['checked_issues']}"
    )
