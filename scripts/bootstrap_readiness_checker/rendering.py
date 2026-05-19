from __future__ import annotations

from typing import Any


def render_markdown(payload: dict[str, Any]) -> str:
    lines = [
        "# Bootstrap Readiness",
        "",
        "| Metric | Value |",
        "| --- | --- |",
        f"| issues_open_count | `{payload['issues_open_count']}` |",
        f"| milestones_open_count | `{payload['milestones_open_count']}` |",
        f"| catalog_open_task_count | `{payload['catalog_open_task_count']}` |",
        f"| blockers_open_count | `{payload['blockers_open_count']}` |",
        f"| readiness_state | `{payload['readiness_state']}` |",
        f"| intake_recommendation | `{payload['intake_recommendation']}` |",
    ]
    return "\n".join(lines) + "\n"
