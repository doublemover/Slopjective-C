"""Data models for remaining spec task seed configuration."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class SeedToolingConfig:
    expected_task_count: int
    repo_default: str
    catalog_md_default: Path
    catalog_json_default: Path
    sleep_seconds_default: float
    lane_name: dict[str, str]
    conformance_milestone_by_tag: dict[str, str]
    lane_milestone_titles: dict[str, str]
    lane_milestone_due_on: dict[str, str]
    label_defs: dict[str, tuple[str, str]]
    planning_lane_by_issue: dict[int, str]


class ConfigError(RuntimeError):
    pass


__all__ = [
    "ConfigError",
    "SeedToolingConfig",
]
