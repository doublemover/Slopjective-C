from __future__ import annotations

import math
from typing import Any

from .constants import (
    LARGE_MAX_IDS,
    MEDIUM_MAX_IDS,
    SIGNAL_PRIORITY,
    SMALL_MAX_IDS,
    SPLIT_MAX_IDS_PER_SHARD,
    SPLIT_TARGET_IDS_PER_SHARD,
)
from .errors import RecommendError
from .models import Recommendation, RecommenderInputs


def classify_base(id_count: int) -> str:
    if 1 <= id_count <= SMALL_MAX_IDS:
        return "small"
    if id_count <= MEDIUM_MAX_IDS:
        return "medium"
    if id_count <= LARGE_MAX_IDS:
        return "large"
    return "oversize"


def validate_inputs(inputs: RecommenderInputs) -> None:
    if inputs.id_count < 1:
        raise RecommendError("id_count must be >= 1")
    if inputs.primary_files < 1:
        raise RecommendError("primary_files must be >= 1")
    if inputs.hard_dependency_count < 0:
        raise RecommendError("hard_dependency_count must be >= 0")
    if inputs.lane_footprint < 1:
        raise RecommendError("lane_footprint must be >= 1")


def triggered_signals(
    *,
    primary_files: int,
    hard_dependency_count: int,
    lane_footprint: int,
    primary_files_limit: int,
    hard_dependency_limit: int,
    lane_footprint_limit: int,
) -> list[str]:
    signals = {
        "primary_files": primary_files > primary_files_limit,
        "hard_dependency_count": hard_dependency_count > hard_dependency_limit,
        "lane_footprint": lane_footprint > lane_footprint_limit,
    }
    return [name for name in SIGNAL_PRIORITY if signals[name]]


def promote_small_if_needed(
    inputs: RecommenderInputs,
    rule_trace: list[str],
    rule_drivers: dict[str, dict[str, Any]],
) -> str:
    small_triggers = triggered_signals(
        primary_files=inputs.primary_files,
        hard_dependency_count=inputs.hard_dependency_count,
        lane_footprint=inputs.lane_footprint,
        primary_files_limit=1,
        hard_dependency_limit=1,
        lane_footprint_limit=1,
    )
    if not small_triggers:
        return "small"
    rule_trace.append("SZ-R2")
    rule_drivers["SZ-R2"] = {
        "triggered_signals": small_triggers,
        "selected_signal": small_triggers[0],
    }
    return "medium"


def promote_medium_if_needed(
    inputs: RecommenderInputs,
    current_class: str,
    rule_trace: list[str],
    rule_drivers: dict[str, dict[str, Any]],
) -> str:
    if current_class != "medium":
        return current_class
    medium_triggers = triggered_signals(
        primary_files=inputs.primary_files,
        hard_dependency_count=inputs.hard_dependency_count,
        lane_footprint=inputs.lane_footprint,
        primary_files_limit=2,
        hard_dependency_limit=3,
        lane_footprint_limit=2,
    )
    if not medium_triggers:
        return current_class
    rule_trace.append("SZ-R3")
    rule_drivers["SZ-R3"] = {
        "triggered_signals": medium_triggers,
        "selected_signal": medium_triggers[0],
    }
    return "large"


def large_complexity_requires_split(
    inputs: RecommenderInputs,
    rule_trace: list[str],
    rule_drivers: dict[str, dict[str, Any]],
) -> bool:
    large_triggers = triggered_signals(
        primary_files=inputs.primary_files,
        hard_dependency_count=inputs.hard_dependency_count,
        lane_footprint=inputs.lane_footprint,
        primary_files_limit=4,
        hard_dependency_limit=6,
        lane_footprint_limit=2,
    )
    if not large_triggers:
        return False
    rule_trace.append("SZ-R4")
    rule_drivers["SZ-R4"] = {
        "triggered_signals": large_triggers,
        "selected_signal": large_triggers[0],
    }
    return True


def calculate_split_size(id_count: int) -> tuple[int, int, str]:
    preferred_count = math.ceil(id_count / SPLIT_TARGET_IDS_PER_SHARD)
    max_size_count = math.ceil(id_count / SPLIT_MAX_IDS_PER_SHARD)
    floor_count = 2
    recommended_shard_count = max(preferred_count, max_size_count, floor_count)
    target_ids_per_shard = math.ceil(id_count / recommended_shard_count)
    if target_ids_per_shard > SPLIT_MAX_IDS_PER_SHARD:
        raise RecommendError(
            "split policy invariant violated: target_ids_per_shard exceeded 23"
        )

    split_count_driver = "minimum-two-shards-floor"
    if recommended_shard_count == preferred_count and preferred_count >= floor_count:
        split_count_driver = "target-ids-per-shard<=20"
    elif recommended_shard_count == max_size_count and max_size_count >= floor_count:
        split_count_driver = "target-ids-per-shard<=23"
    return recommended_shard_count, target_ids_per_shard, split_count_driver


def recommend(inputs: RecommenderInputs) -> Recommendation:
    validate_inputs(inputs)

    base_class = classify_base(inputs.id_count)
    recommended_class = "large" if base_class == "oversize" else base_class
    split_required = False
    rule_trace: list[str] = ["SZ-R1"]
    rule_drivers: dict[str, dict[str, Any]] = {}

    if base_class == "small":
        recommended_class = promote_small_if_needed(inputs, rule_trace, rule_drivers)

    recommended_class = promote_medium_if_needed(
        inputs,
        recommended_class,
        rule_trace,
        rule_drivers,
    )

    if recommended_class == "large":
        split_required = large_complexity_requires_split(
            inputs,
            rule_trace,
            rule_drivers,
        )

    if inputs.id_count > LARGE_MAX_IDS:
        split_required = True
        rule_trace.append("SZ-R5")

    recommended_shard_count = 1
    target_ids_per_shard = inputs.id_count
    split_count_driver: str | None = None
    if split_required:
        rule_trace.append("SZ-R6")
        (
            recommended_shard_count,
            target_ids_per_shard,
            split_count_driver,
        ) = calculate_split_size(inputs.id_count)

    return Recommendation(
        base_class=base_class,
        recommended_class=recommended_class,
        split_required=split_required,
        rule_trace=tuple(rule_trace),
        recommended_shard_count=recommended_shard_count,
        target_ids_per_shard=target_ids_per_shard,
        rule_drivers=rule_drivers,
        split_count_driver=split_count_driver,
    )
