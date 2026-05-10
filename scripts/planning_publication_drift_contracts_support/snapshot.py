from __future__ import annotations

from typing import Any, Mapping

from .models import JsonObject


def publication_snapshot(report: Mapping[str, Any]) -> JsonObject:
    failures = report.get("failures", [])
    failure_count = len(failures) if isinstance(failures, list) else failures
    return {
        "repository": report["repository"],
        "updatedAtUtc": report["updatedAtUtc"],
        "milestoneCount": report["milestoneCount"],
        "issueCount": report["issueCount"],
        "dependencyCount": report["dependencyCount"],
        "failures": failure_count,
        "milestones": report["milestones"],
        "issues": report["issues"],
        "dependencies": report["dependencies"],
    }
