from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class RecommenderInputs:
    id_count: int
    primary_files: int
    hard_dependency_count: int
    lane_footprint: int


@dataclass(frozen=True)
class Recommendation:
    base_class: str
    recommended_class: str
    split_required: bool
    rule_trace: tuple[str, ...]
    recommended_shard_count: int
    target_ids_per_shard: int
    rule_drivers: dict[str, dict[str, Any]]
    split_count_driver: str | None
