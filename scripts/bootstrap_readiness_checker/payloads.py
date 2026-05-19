from __future__ import annotations

from typing import Any


def build_payload(
    *,
    issues_open_count: int,
    milestones_open_count: int,
    catalog_open_task_count: int,
    blockers_open_count: int,
) -> dict[str, Any]:
    blocking_dimensions = [
        field_name
        for field_name, count in (
            ("issues_open_count", issues_open_count),
            ("milestones_open_count", milestones_open_count),
            ("catalog_open_task_count", catalog_open_task_count),
            ("blockers_open_count", blockers_open_count),
        )
        if count > 0
    ]
    readiness_state = "bootstrappable" if not blocking_dimensions else "blocked"
    intake_recommendation = "go" if readiness_state == "bootstrappable" else "hold"
    return {
        "issues_open_count": issues_open_count,
        "milestones_open_count": milestones_open_count,
        "catalog_open_task_count": catalog_open_task_count,
        "blockers_open_count": blockers_open_count,
        "readiness_state": readiness_state,
        "intake_recommendation": intake_recommendation,
        "blocking_dimensions": blocking_dimensions,
    }
