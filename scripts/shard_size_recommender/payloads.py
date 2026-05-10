from __future__ import annotations

from typing import Any

from .constants import SIGNAL_PRIORITY
from .models import Recommendation, RecommenderInputs


def build_payload(
    *,
    inputs: RecommenderInputs,
    recommendation: Recommendation,
) -> dict[str, Any]:
    return {
        "contract_id": "V013-TOOL-04-SHARD-SIZE-v1",
        "calibration_sources": [
            (
                "docs/reference/legacy_spec_anchor_index.md"
                "#planning-v012-wave10-candidate-shards-20260223"
            ),
            (
                "docs/reference/legacy_spec_anchor_index.md"
                "#planning-v012-wave16-candidate-shards-20260223"
            ),
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
                    "retired route target_ids_per_shard<=23",
                    "if split_required and count resolves to 1, force 2 shards",
                ],
                "selected_split_count_driver": recommendation.split_count_driver,
            },
        },
    }
