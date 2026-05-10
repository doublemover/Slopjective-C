"""Output rendering for the remaining spec task seed tool."""

from __future__ import annotations

import json
from datetime import date

from remaining_task_extraction.seed_review import ReviewedTask
from remaining_task_extraction.seed_review import format_source_reference


def render_catalog_markdown(
    tasks: list[ReviewedTask],
    lane_name: dict[str, str],
    generated_on: date,
) -> str:
    lane_counts: dict[str, int] = {"A": 0, "B": 0, "C": 0, "D": 0}
    bucket_counts: dict[str, int] = {}
    quality_counts: dict[str, int] = {}

    for task in tasks:
        lane_counts[task.lane] = lane_counts.get(task.lane, 0) + 1
        bucket_counts[task.bucket] = bucket_counts.get(task.bucket, 0) + 1
        quality_counts[task.quality] = quality_counts.get(task.quality, 0) + 1

    lines: list[str] = [
        "# Remaining Task Review Catalog (510-Task Sweep)",
        "",
        f"_Generated on {generated_on.isoformat()} by scripts/seed_remaining_spec_tasks.py._",
        "",
        "## Coverage Summary",
        "",
        f"- Total reviewed tasks: **{len(tasks)}**",
        f"- Bucket counts: conformance-checklist **{bucket_counts.get('conformance-checklist', 0)}**, release-evidence **{bucket_counts.get('release-evidence', 0)}**, planning-checklist **{bucket_counts.get('planning-checklist', 0)}**",
        f"- Lane counts: A **{lane_counts.get('A', 0)}**, B **{lane_counts.get('B', 0)}**, C **{lane_counts.get('C', 0)}**, D **{lane_counts.get('D', 0)}**",
        f"- Definition quality counts: strong **{quality_counts.get('strong', 0)}**, medium **{quality_counts.get('medium', 0)}**, weak **{quality_counts.get('weak', 0)}**",
        "",
        "## Parallel Worklane Guidance",
        "",
        "- Lane A: normative/spec closure tasks with cross-part semantic contracts.",
        "- Lane B: implementation/tooling and conformance automation tasks.",
        "- Lane C: governance, extension process, and ecosystem policy tasks.",
        "- Lane D: release readiness, checkpoints, handoff, and operational control tasks.",
        "",
        "## Task Reviews",
        "",
    ]

    for task in tasks:
        lines.append(f"### {task.task_id} - {task.title}")
        lines.append("")
        lines.append(f"- Source: `{format_source_reference(task)}`")
        lines.append(f"- Bucket: `{task.bucket}`")
        lines.append(f"- Lane: `Lane {task.lane} - {lane_name[task.lane]}`")
        lines.append(f"- Milestone: `{task.milestone_title}`")
        lines.append(f"- Shard: `{task.shard}`")
        lines.append(f"- Original checkbox: `{task.original}`")
        lines.append(f"- Review quality: `{task.quality}`")
        if task.quality_gaps:
            lines.append("- Gaps identified:")
            for gap in task.quality_gaps:
                lines.append(f"  - {gap}")
        else:
            lines.append("- Gaps identified: none; wording already has strong structure.")
        lines.append(f"- Improved objective: {task.objective}")
        lines.append("- Improved deliverables:")
        for item in task.deliverables:
            lines.append(f"  - {item}")
        lines.append("- Improved acceptance criteria:")
        for item in task.acceptance_criteria:
            lines.append(f"  - {item}")
        lines.append("- Dependencies:")
        for item in task.dependencies:
            lines.append(f"  - {item}")
        lines.append("- Validation commands:")
        for cmd in task.validation_commands:
            lines.append(f"  - `{cmd}`")
        lines.append("")

    return "\n".join(lines).rstrip() + "\n"


def serialize_catalog_json(
    tasks: list[ReviewedTask],
    lane_name: dict[str, str],
    generated_on: date,
) -> str:
    payload = {
        "generated_on": generated_on.isoformat(),
        "task_count": len(tasks),
        "tasks": [
            {
                "task_id": task.task_id,
                "task_key": task.task_key,
                "source_line_hash": task.source_line_hash,
                "title": task.title,
                "path": task.path,
                "line": task.line,
                "bucket": task.bucket,
                "lane": task.lane,
                "lane_name": lane_name[task.lane],
                "milestone_title": task.milestone_title,
                "priority_label": task.priority_label,
                "area_label": task.area_label,
                "type_label": task.type_label,
                "labels": task.labels,
                "quality": task.quality,
                "quality_gaps": task.quality_gaps,
                "original": task.original,
                "cleaned": task.cleaned,
                "objective": task.objective,
                "deliverables": task.deliverables,
                "acceptance_criteria": task.acceptance_criteria,
                "dependencies": task.dependencies,
                "validation_commands": task.validation_commands,
                "shard": task.shard,
                "execution_status": task.execution_status,
            }
            for task in tasks
        ],
    }
    return json.dumps(payload, indent=2) + "\n"


def build_issue_body(task: ReviewedTask, lane_name: dict[str, str]) -> str:
    quality_line = {
        "strong": "Strong baseline wording; improvements focus on execution rigor and evidence shape.",
        "medium": "Partially defined; improvements add missing acceptance and validation precision.",
        "weak": "Underspecified; improvements convert this into an outcome-driven implementation task.",
    }[task.quality]

    gap_lines = task.quality_gaps or [
        "No structural gaps detected in source wording; issue still adds deterministic execution metadata."
    ]

    parallel_with = ", ".join(lane for lane in ["A", "B", "C", "D"] if lane != task.lane)

    body = [
        "## Source Traceability",
        f"- Task ID: `{task.task_id}`",
        f"- Source: `{format_source_reference(task)}`",
        f"- Source bucket: `{task.bucket}`",
        f"- Original checkbox: `{task.original}`",
        "",
        "## Definition Review",
        f"- Assessment: **{task.quality.upper()}**",
        f"- Summary: {quality_line}",
        "- What I would do differently:",
    ]

    for gap in gap_lines:
        body.append(f"  - {gap}")

    body.extend(
        [
            "",
            "## Improved Task Definition",
            f"### Objective\n{task.objective}",
            "",
            "### Deliverables",
        ]
    )
    for item in task.deliverables:
        body.append(f"- {item}")

    body.append("")
    body.append("### Acceptance Criteria")
    for idx, item in enumerate(task.acceptance_criteria, start=1):
        body.append(f"{idx}. {item}")

    body.append("")
    body.append("### Dependencies")
    for item in task.dependencies:
        body.append(f"- {item}")

    body.append("")
    body.append("### Validation Commands")
    for cmd in task.validation_commands:
        body.append(f"- `{cmd}`")

    body.extend(
        [
            "",
            "## Parallel Execution Plan",
            f"- Primary lane: **Lane {task.lane} - {lane_name[task.lane]}**",
            f"- Parallelizable with lanes: **{parallel_with}**",
            f"- Suggested shard key: `{task.shard}` (batch same-shard tasks to reduce context switching)",
            "",
            "## Closeout Evidence Required",
            "- Commit SHA(s) implementing the task",
            "- Paths to produced artifacts/tests/evidence",
            "- Validation command outputs",
            "- Checklist row update reference",
        ]
    )

    return "\n".join(body).strip() + "\n"
