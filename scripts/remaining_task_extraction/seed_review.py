from __future__ import annotations

from remaining_task_extraction.seed_config import SeedToolingConfig
from remaining_task_extraction.seed_review_inference import (
    derive_shard,
    infer_area_label,
    infer_bucket,
    infer_lane,
    infer_milestone_title,
    infer_priority_label,
    infer_type_label,
)
from remaining_task_extraction.seed_review_models import (
    DEFAULT_EXECUTION_STATUS,
    RawTask,
    ReviewedTask,
)
from remaining_task_extraction.seed_review_quality import (
    build_acceptance_criteria,
    build_deliverables,
    build_dependencies,
    build_labels,
    build_objective,
    build_validation_commands,
    evaluate_definition,
)
from remaining_task_extraction.seed_review_text import (
    CODE_PATTERN,
    ISSUE_REF_PATTERN,
    LINK_PATTERN,
    PLANNING_ISSUE_PATTERN,
    SPACES_PATTERN,
    TASK_REF_PATTERN,
    build_source_line_hash,
    build_task_key,
    build_title,
    clean_markdown_text,
    format_source_reference,
    normalize_title,
)


def review_tasks(raw_tasks: list[RawTask], config: SeedToolingConfig) -> list[ReviewedTask]:
    reviewed: list[ReviewedTask] = []
    for idx, raw in enumerate(raw_tasks, start=1):
        task_id = f"SPT-{idx:04d}"
        task_key = build_task_key(raw)
        source_line_hash = build_source_line_hash(raw)
        cleaned = clean_markdown_text(raw.text)
        bucket = infer_bucket(raw)
        lane = infer_lane(raw, bucket, config.planning_lane_by_issue)
        milestone_title = infer_milestone_title(
            raw,
            bucket,
            lane,
            config.conformance_milestone_by_tag,
            config.lane_milestone_titles,
        )
        priority_label = infer_priority_label(raw, bucket, lane)
        area_label = infer_area_label(raw, cleaned, lane, bucket)
        type_label = infer_type_label(bucket)
        quality, gaps = evaluate_definition(raw, cleaned)
        objective = build_objective(cleaned)
        deliverables = build_deliverables(raw, bucket)
        acceptance_criteria = build_acceptance_criteria(raw, bucket)
        dependencies = build_dependencies(raw, lane, bucket, config.lane_name)
        validation_commands = build_validation_commands(bucket)
        shard = derive_shard(raw)
        labels = build_labels(
            lane=lane,
            bucket=bucket,
            priority_label=priority_label,
            area_label=area_label,
            type_label=type_label,
        )
        title = build_title(task_id, lane, cleaned)

        reviewed.append(
            ReviewedTask(
                task_id=task_id,
                task_key=task_key,
                source_line_hash=source_line_hash,
                path=raw.path,
                line=raw.line,
                original=raw.text,
                cleaned=cleaned,
                bucket=bucket,
                lane=lane,
                milestone_title=milestone_title,
                priority_label=priority_label,
                area_label=area_label,
                type_label=type_label,
                labels=labels,
                quality=quality,
                quality_gaps=gaps,
                objective=objective,
                deliverables=deliverables,
                acceptance_criteria=acceptance_criteria,
                dependencies=dependencies,
                validation_commands=validation_commands,
                shard=shard,
                title=title,
                execution_status=DEFAULT_EXECUTION_STATUS,
            )
        )

    return reviewed


__all__ = [
    "CODE_PATTERN",
    "DEFAULT_EXECUTION_STATUS",
    "ISSUE_REF_PATTERN",
    "LINK_PATTERN",
    "PLANNING_ISSUE_PATTERN",
    "RawTask",
    "ReviewedTask",
    "SPACES_PATTERN",
    "TASK_REF_PATTERN",
    "build_acceptance_criteria",
    "build_deliverables",
    "build_dependencies",
    "build_labels",
    "build_objective",
    "build_source_line_hash",
    "build_task_key",
    "build_title",
    "build_validation_commands",
    "clean_markdown_text",
    "derive_shard",
    "evaluate_definition",
    "format_source_reference",
    "infer_area_label",
    "infer_bucket",
    "infer_lane",
    "infer_milestone_title",
    "infer_priority_label",
    "infer_type_label",
    "normalize_title",
    "review_tasks",
]
