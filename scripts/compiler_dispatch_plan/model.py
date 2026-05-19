from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class IssueRow:
    number: int
    title: str
    milestone_number: int
    milestone_title: str
    lane_labels: tuple[str, ...]


@dataclass(frozen=True)
class TaskRef:
    issue_number: int
    task_id: str
    sequence: int
    title: str
