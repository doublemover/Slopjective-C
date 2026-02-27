#!/usr/bin/env python3
"""Recommend deterministic shard sizing using v0.12 Wave 10/16 calibration."""

from __future__ import annotations

import argparse
import json
import math
import sys
from dataclasses import dataclass
from typing import Any

SMALL_MAX_IDS = 4
MEDIUM_MAX_IDS = 12
LARGE_MAX_IDS = 30

SPLIT_TARGET_IDS_PER_SHARD = 20
SPLIT_MAX_IDS_PER_SHARD = 23

SIGNAL_PRIORITY = ("primary_files", "hard_dependency_count", "lane_footprint")


class RecommendError(ValueError):
    """Raised when recommender inputs are invalid."""


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


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="recommend_shard_size.py",
        description=(
            "Evaluate deterministic shard-size guidance calibrated from v0.12 "
            "Wave 10 and Wave 16 execution history."
        ),
    )
    parser.add_argument(
        "--id-count",
        type=int,
        required=True,
        help="Planned checklist ID count for the shard candidate (must be >= 1).",
    )
    parser.add_argument(
        "--primary-files",
        type=int,
        required=True,
        help="Primary artifact file count expected to be edited (must be >= 1).",
    )
    parser.add_argument(
        "--hard-dependency-count",
        type=int,
        required=True,
        help="Hard predecessor dependency count (must be >= 0).",
    )
    parser.add_argument(
        "--lane-footprint",
        type=int,
        required=True,
        help="Distinct worklane count touched by the shard candidate (must be >= 1).",
    )
    parser.add_argument(
        "--format",
        choices=("json", "text"),
        default="json",
        help="Render output as machine-readable JSON or compact text summary.",
    )
    return parser


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


def recommend(inputs: RecommenderInputs) -> Recommendation:
    validate_inputs(inputs)

    base_class = classify_base(inputs.id_count)
    recommended_class = "large" if base_class == "oversize" else base_class
    split_required = False
    rule_trace: list[str] = ["SZ-R1"]
    rule_drivers: dict[str, dict[str, Any]] = {}

    if base_class == "small":
        small_triggers = triggered_signals(
            primary_files=inputs.primary_files,
            hard_dependency_count=inputs.hard_dependency_count,
            lane_footprint=inputs.lane_footprint,
            primary_files_limit=1,
            hard_dependency_limit=1,
            lane_footprint_limit=1,
        )
        if small_triggers:
            recommended_class = "medium"
            rule_trace.append("SZ-R2")
            rule_drivers["SZ-R2"] = {
                "triggered_signals": small_triggers,
                "selected_signal": small_triggers[0],
            }

    if recommended_class == "medium":
        medium_triggers = triggered_signals(
            primary_files=inputs.primary_files,
            hard_dependency_count=inputs.hard_dependency_count,
            lane_footprint=inputs.lane_footprint,
            primary_files_limit=2,
            hard_dependency_limit=3,
            lane_footprint_limit=2,
        )
        if medium_triggers:
            recommended_class = "large"
            rule_trace.append("SZ-R3")
            rule_drivers["SZ-R3"] = {
                "triggered_signals": medium_triggers,
                "selected_signal": medium_triggers[0],
            }

    if recommended_class == "large":
        large_triggers = triggered_signals(
            primary_files=inputs.primary_files,
            hard_dependency_count=inputs.hard_dependency_count,
            lane_footprint=inputs.lane_footprint,
            primary_files_limit=4,
            hard_dependency_limit=6,
            lane_footprint_limit=2,
        )
        if large_triggers:
            split_required = True
            rule_trace.append("SZ-R4")
            rule_drivers["SZ-R4"] = {
                "triggered_signals": large_triggers,
                "selected_signal": large_triggers[0],
            }

    if inputs.id_count > LARGE_MAX_IDS:
        split_required = True
        rule_trace.append("SZ-R5")

    recommended_shard_count = 1
    target_ids_per_shard = inputs.id_count
    split_count_driver: str | None = None
    if split_required:
        rule_trace.append("SZ-R6")
        preferred_count = math.ceil(inputs.id_count / SPLIT_TARGET_IDS_PER_SHARD)
        max_size_count = math.ceil(inputs.id_count / SPLIT_MAX_IDS_PER_SHARD)
        floor_count = 2
        recommended_shard_count = max(preferred_count, max_size_count, floor_count)
        target_ids_per_shard = math.ceil(inputs.id_count / recommended_shard_count)
        if target_ids_per_shard > SPLIT_MAX_IDS_PER_SHARD:
            raise RecommendError(
                "split policy invariant violated: target_ids_per_shard exceeded 23"
            )

        split_count_driver = "minimum-two-shards-floor"
        if recommended_shard_count == preferred_count and preferred_count >= floor_count:
            split_count_driver = "target-ids-per-shard<=20"
        elif recommended_shard_count == max_size_count and max_size_count >= floor_count:
            split_count_driver = "target-ids-per-shard<=23"

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


def build_payload(
    *,
    inputs: RecommenderInputs,
    recommendation: Recommendation,
) -> dict[str, Any]:
    return {
        "contract_id": "V013-TOOL-04-SHARD-SIZE-v1",
        "calibration_sources": [
            "spec/planning/v012_wave10_candidate_shards_20260223.md",
            "spec/planning/v012_wave16_candidate_shards_20260223.md",
        ],
        "inputs": {
            "id_count": inputs.id_count,
            "primary_files": inputs.primary_files,
            "hard_dependency_count": inputs.hard_dependency_count,
            "lane_footprint": inputs.lane_footprint,
        },
        "assumptions": {
            "historical_baseline": "single-file ownership and single-lane execution",
            "threshold_bands": {
                "small": "1..4",
                "medium": "5..12",
                "large": "13..30",
                "oversize": ">30",
            },
        },
        "recommended_class": recommendation.recommended_class,
        "split_required": recommendation.split_required,
        "rule_trace": ",".join(recommendation.rule_trace),
        "recommended_shard_count": recommendation.recommended_shard_count,
        "target_ids_per_shard": recommendation.target_ids_per_shard,
        "diagnostics": {
            "base_class": recommendation.base_class,
            "rule_drivers": recommendation.rule_drivers,
            "tie_break_policy": {
                "signal_priority": list(SIGNAL_PRIORITY),
                "split_count_priority": [
                    "prefer target_ids_per_shard<=20",
                    "fallback target_ids_per_shard<=23",
                    "if split_required and count resolves to 1, force 2 shards",
                ],
                "selected_split_count_driver": recommendation.split_count_driver,
            },
        },
    }


def render_text(payload: dict[str, Any]) -> str:
    lines = [
        f"contract_id: {payload['contract_id']}",
        f"recommended_class: {payload['recommended_class']}",
        f"split_required: {payload['split_required']}",
        f"rule_trace: {payload['rule_trace']}",
        f"recommended_shard_count: {payload['recommended_shard_count']}",
        f"target_ids_per_shard: {payload['target_ids_per_shard']}",
    ]
    return "\n".join(lines) + "\n"


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    inputs = RecommenderInputs(
        id_count=args.id_count,
        primary_files=args.primary_files,
        hard_dependency_count=args.hard_dependency_count,
        lane_footprint=args.lane_footprint,
    )

    try:
        recommendation = recommend(inputs)
        payload = build_payload(inputs=inputs, recommendation=recommendation)
    except RecommendError as exc:
        print(f"recommend-shard-size: {exc}", file=sys.stderr)
        return 2

    if args.format == "text":
        sys.stdout.write(render_text(payload))
    else:
        sys.stdout.write(json.dumps(payload, indent=2) + "\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
