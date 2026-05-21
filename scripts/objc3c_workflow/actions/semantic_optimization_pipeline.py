"""Semantic optimization pipeline workflow action."""

from __future__ import annotations

import sys

from ..commands import run

SEMANTIC_OPTIMIZATION_PIPELINE_ACTION = "validate-semantic-optimization-pipeline"
SEMANTIC_OPTIMIZATION_PIPELINE_SUMMARY = (
    "validate semantic-preserving optimization pipeline pass order and fail-closed contracts"
)
SEMANTIC_OPTIMIZATION_PIPELINE_BACKEND = (
    "python:scripts/check_objc3c_semantic_optimization_pipeline.py"
)
SEMANTIC_OPTIMIZATION_PIPELINE_VALIDATION_TIER = "policy"
SEMANTIC_OPTIMIZATION_PIPELINE_GUARANTEE_OWNER = (
    "Objective-C 3.0 optimization passes stay typed, deterministic, explicitly "
    "invalidated, post-verified, benchmark-governed, and fail-closed"
)
SEMANTIC_OPTIMIZATION_PIPELINE_COMMAND = (
    sys.executable,
    "scripts/check_objc3c_semantic_optimization_pipeline.py",
)


def action_validate_semantic_optimization_pipeline(_: list[str]) -> int:
    return run([str(part) for part in SEMANTIC_OPTIMIZATION_PIPELINE_COMMAND])


__all__ = [
    "SEMANTIC_OPTIMIZATION_PIPELINE_ACTION",
    "SEMANTIC_OPTIMIZATION_PIPELINE_BACKEND",
    "SEMANTIC_OPTIMIZATION_PIPELINE_GUARANTEE_OWNER",
    "SEMANTIC_OPTIMIZATION_PIPELINE_SUMMARY",
    "SEMANTIC_OPTIMIZATION_PIPELINE_VALIDATION_TIER",
    "action_validate_semantic_optimization_pipeline",
]
