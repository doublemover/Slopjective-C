"""Shard-size recommendation policy package."""

from __future__ import annotations

from .cli import build_parser, main
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
from .payloads import build_payload
from .policy import classify_base, recommend, triggered_signals, validate_inputs
from .rendering import render_text


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
