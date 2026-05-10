from __future__ import annotations

from typing import Any


def render_markdown(payload: dict[str, Any]) -> str:
    milestone = payload["milestone"]
    parallelization = payload["parallelization"]
    lanes = payload["lanes"]

    lines: list[str] = [
        "# Compiler Dispatch Plan",
        "",
        "## Hard-Cutover Owners",
        "",
        f"- Dispatch owner: `{payload['owner_contract']['dispatch_owner']}`",
        f"- Snapshot owner: `{payload['owner_contract']['snapshot_owner']}`",
        f"- Result owner: `{payload['owner_contract']['result_owner']}`",
        f"- Artifact owner: `{payload['owner_contract']['artifact_owner']}`",
        f"- Status owner: `{payload['owner_contract']['status_owner']}`",
        (
            "- Retired retired-route/evidence-log claims disallowed: "
            f"**{str(payload['owner_contract']['no_retired_route_or_evidence_log_claims']).lower()}**"
        ),
        "",
        "## Milestone",
        "",
        f"- Number: **{milestone['number']}**",
        f"- Title: **{milestone['title']}**",
        f"- Open issues in milestone: **{milestone['open_issue_count']}**",
        "",
        "## Parallelization",
        "",
        (
            "- Parallel lanes: "
            + ", ".join(f"`{lane}`" for lane in parallelization["parallel_lanes"])
        ),
        (
            "- Regroup dependency issues: "
            + (
                ", ".join(f"`#{number}`" for number in parallelization["regroup_dependency"])
                if parallelization["regroup_dependency"]
                else "_none_"
            )
        ),
        f"- Note: {parallelization['regroup_dependency_note']}",
        "",
        "## Next Tasks",
        "",
    ]

    for lane_summary in lanes:
        lane = lane_summary["lane"]
        lines.append(
            f"### Lane `{lane}` ({lane_summary['open_issue_count']} open issues)"
        )
        next_tasks = lane_summary["next_tasks"]
        if next_tasks:
            for task in next_tasks:
                lines.append(
                    f"- `#{task['issue_number']}` `{task['task_id']}`: {task['title']}"
                )
        else:
            lines.append("- _No parseable lane task IDs found._")
        lines.append("")
    return "\n".join(lines).rstrip() + "\n"
