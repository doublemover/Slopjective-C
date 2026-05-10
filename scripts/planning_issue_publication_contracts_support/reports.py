from __future__ import annotations

from typing import Any

from .constants import REPORT_CONTRACT_ID
from .dependencies import build_dependency_report
from .time_utils import utc_now


def build_report(
    payload: dict[str, Any],
    existing_report: dict[str, Any],
    milestone_report: dict[str, Any],
    issue_report: dict[str, Any],
) -> dict[str, Any]:
    dependencies = build_dependency_report(payload, issue_report)
    return {
        "contract_id": REPORT_CONTRACT_ID,
        "repository": payload["repository"],
        "startedAtUtc": existing_report.get("startedAtUtc", utc_now()),
        "updatedAtUtc": utc_now(),
        "milestoneCount": len(payload["milestones"]),
        "issueCount": len(payload["issues"]),
        "dependencyCount": len(payload.get("dependencies", [])),
        "milestones": milestone_report,
        "issues": issue_report,
        "dependencies": dependencies,
        "failures": [],
    }


def build_dry_run_report(
    payload: dict[str, Any],
    existing_report: dict[str, Any],
) -> dict[str, Any]:
    milestone_report = dict(existing_report.get("milestones", {}))
    issue_report = dict(existing_report.get("issues", {}))
    return build_report(payload, existing_report, milestone_report, issue_report)
