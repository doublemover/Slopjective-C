from __future__ import annotations

from dataclasses import dataclass

DEFAULT_EXECUTION_STATUS = "open"


@dataclass(frozen=True)
class RawTask:
    path: str
    line: int
    text: str
    origin_path: str


@dataclass(frozen=True)
class ReviewedTask:
    task_id: str
    task_key: str
    source_line_hash: str
    path: str
    line: int
    original: str
    cleaned: str
    bucket: str
    lane: str
    milestone_title: str
    priority_label: str
    area_label: str
    type_label: str
    labels: list[str]
    quality: str
    quality_gaps: list[str]
    objective: str
    deliverables: list[str]
    acceptance_criteria: list[str]
    dependencies: list[str]
    validation_commands: list[str]
    shard: str
    title: str
    execution_status: str
