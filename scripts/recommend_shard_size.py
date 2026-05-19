#!/usr/bin/env python3
"""Recommend deterministic shard sizing using v0.12 Wave 10/16 calibration."""

from __future__ import annotations

try:
    from shard_size_recommender import (
        LARGE_MAX_IDS,
        MEDIUM_MAX_IDS,
        SIGNAL_PRIORITY,
        SMALL_MAX_IDS,
        SPLIT_MAX_IDS_PER_SHARD,
        SPLIT_TARGET_IDS_PER_SHARD,
        Recommendation,
        RecommendError,
        RecommenderInputs,
        build_parser,
        build_payload,
        classify_base,
        main,
        recommend,
        render_text,
        triggered_signals,
        validate_inputs,
    )
except ModuleNotFoundError:
    from scripts.shard_size_recommender import (
        LARGE_MAX_IDS,
        MEDIUM_MAX_IDS,
        SIGNAL_PRIORITY,
        SMALL_MAX_IDS,
        SPLIT_MAX_IDS_PER_SHARD,
        SPLIT_TARGET_IDS_PER_SHARD,
        Recommendation,
        RecommendError,
        RecommenderInputs,
        build_parser,
        build_payload,
        classify_base,
        main,
        recommend,
        render_text,
        triggered_signals,
        validate_inputs,
    )


__all__ = [
    "LARGE_MAX_IDS",
    "MEDIUM_MAX_IDS",
    "SIGNAL_PRIORITY",
    "SMALL_MAX_IDS",
    "SPLIT_MAX_IDS_PER_SHARD",
    "SPLIT_TARGET_IDS_PER_SHARD",
    "Recommendation",
    "RecommendError",
    "RecommenderInputs",
    "build_parser",
    "build_payload",
    "classify_base",
    "main",
    "recommend",
    "render_text",
    "triggered_signals",
    "validate_inputs",
]


if __name__ == "__main__":
    raise SystemExit(main())
