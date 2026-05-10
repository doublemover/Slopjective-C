from __future__ import annotations

from typing import Any, Sequence

from objc3c_tooling.paths import display_path

from compiler_dispatch_plan.constants import COMPILER_DISPATCH_CONTRACT_ID
from compiler_dispatch_plan.constants import DEFAULT_ISSUES_JSON
from compiler_dispatch_plan.constants import LANE_ORDER
from compiler_dispatch_plan.constants import TASK_ID_RE
from compiler_dispatch_plan.model import IssueRow, TaskRef
from compiler_dispatch_plan.owner_contract import dispatch_owner_contract


def pick_target_milestone(rows: Sequence[IssueRow], explicit_number: int | None) -> int:
    if explicit_number is not None:
        matches = {row.milestone_number for row in rows if row.milestone_number == explicit_number}
        if not matches:
            raise ValueError(
                f"milestone {explicit_number} not present in provided open issue snapshot"
            )
        return explicit_number

    return min(row.milestone_number for row in rows)


def parse_task_ref(row: IssueRow, lane: str) -> TaskRef | None:
    match = TASK_ID_RE.search(row.title)
    if not match:
        return None
    parsed_lane = match.group("lane")
    if parsed_lane != lane:
        return None
    task_id = f"M{int(match.group('milestone')):02d}-{parsed_lane}{match.group('seq')}"
    return TaskRef(
        issue_number=row.number,
        task_id=task_id,
        sequence=int(match.group("seq")),
        title=row.title,
    )


def build_payload(rows: Sequence[IssueRow], *, milestone_number: int, top_n: int) -> dict[str, Any]:
    target_rows = [row for row in rows if row.milestone_number == milestone_number]
    if not target_rows:
        raise ValueError(f"no rows available for milestone {milestone_number}")

    milestone_title = sorted({row.milestone_title for row in target_rows}, key=str.casefold)[0]
    lane_summaries: list[dict[str, Any]] = []
    for lane in LANE_ORDER:
        lane_rows = [row for row in target_rows if f"lane:{lane}" in row.lane_labels]
        tasks: list[TaskRef] = []
        for row in lane_rows:
            parsed = parse_task_ref(row, lane)
            if parsed is not None:
                tasks.append(parsed)
        tasks.sort(key=lambda entry: (entry.sequence, entry.issue_number, entry.task_id))

        lane_summaries.append(
            {
                "lane": lane,
                "open_issue_count": len(lane_rows),
                "next_tasks": [
                    {
                        "issue_number": task.issue_number,
                        "task_id": task.task_id,
                        "sequence": task.sequence,
                        "title": task.title,
                    }
                    for task in tasks[:top_n]
                ],
            }
        )

    int_rows = [row for row in target_rows if "lane:INT" in row.lane_labels]
    int_rows.sort(key=lambda row: (row.number, row.title.casefold(), row.title))
    parallel_lanes = [
        lane_summary["lane"]
        for lane_summary in lane_summaries
        if lane_summary["open_issue_count"] > 0
    ]
    return {
        "contract_id": COMPILER_DISPATCH_CONTRACT_ID,
        "owner_contract": dispatch_owner_contract(),
        "source": {
            "issues_json": display_path(DEFAULT_ISSUES_JSON),
        },
        "milestone": {
            "number": milestone_number,
            "title": milestone_title,
            "open_issue_count": len(target_rows),
        },
        "parallelization": {
            "parallel_lanes": parallel_lanes,
            "regroup_dependency": [row.number for row in int_rows],
            "regroup_dependency_note": (
                "INT regroup is lane-gated and should execute only after parallel lanes complete."
            ),
        },
        "lanes": lane_summaries,
    }
