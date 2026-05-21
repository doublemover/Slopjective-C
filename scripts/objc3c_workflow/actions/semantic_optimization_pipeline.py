"""Semantic optimization pipeline workflow action."""

from __future__ import annotations

import sys

from ..commands import run

SEMANTIC_OPTIMIZATION_PIPELINE_ACTION = "validate-semantic-optimization-pipeline"
OPTIMIZATION_PROOF_MODEL_ACTION = "validate-optimization-proof-model"
SEMANTIC_OPTIMIZATION_PIPELINE_SUMMARY = (
    "validate semantic-preserving optimization pipeline pass order and fail-closed contracts"
)
OPTIMIZATION_PROOF_MODEL_SUMMARY = (
    "validate semantic optimization proof records, verdicts, invalidation, and skip claims"
)
SEMANTIC_OPTIMIZATION_PIPELINE_BACKEND = (
    "python:scripts/check_objc3c_semantic_optimization_pipeline.py"
)
OPTIMIZATION_PROOF_MODEL_BACKEND = (
    "python:scripts/check_objc3c_optimization_proof_model.py"
)
SEMANTIC_OPTIMIZATION_PIPELINE_VALIDATION_TIER = "policy"
OPTIMIZATION_PROOF_MODEL_VALIDATION_TIER = "policy"
SEMANTIC_OPTIMIZATION_PIPELINE_GUARANTEE_OWNER = (
    "Objective-C 3.0 optimization passes stay typed, deterministic, explicitly "
    "invalidated, post-verified, benchmark-governed, and fail-closed"
)
OPTIMIZATION_PROOF_MODEL_GUARANTEE_OWNER = (
    "Objective-C 3.0 optimization passes carry source graph, source-map, "
    "ownership, runtime ABI, package identity, semantic equivalence, "
    "invalidation, and unsupported-skip proof records"
)
SEMANTIC_OPTIMIZATION_PIPELINE_COMMAND = (
    sys.executable,
    "scripts/check_objc3c_semantic_optimization_pipeline.py",
)
OPTIMIZATION_PROOF_MODEL_COMMAND = (
    sys.executable,
    "scripts/check_objc3c_optimization_proof_model.py",
)


def action_validate_semantic_optimization_pipeline(_: list[str]) -> int:
    return run([str(part) for part in SEMANTIC_OPTIMIZATION_PIPELINE_COMMAND])


def action_validate_optimization_proof_model(_: list[str]) -> int:
    return run([str(part) for part in OPTIMIZATION_PROOF_MODEL_COMMAND])


__all__ = [
    "OPTIMIZATION_PROOF_MODEL_ACTION",
    "OPTIMIZATION_PROOF_MODEL_BACKEND",
    "OPTIMIZATION_PROOF_MODEL_GUARANTEE_OWNER",
    "OPTIMIZATION_PROOF_MODEL_SUMMARY",
    "OPTIMIZATION_PROOF_MODEL_VALIDATION_TIER",
    "SEMANTIC_OPTIMIZATION_PIPELINE_ACTION",
    "SEMANTIC_OPTIMIZATION_PIPELINE_BACKEND",
    "SEMANTIC_OPTIMIZATION_PIPELINE_GUARANTEE_OWNER",
    "SEMANTIC_OPTIMIZATION_PIPELINE_SUMMARY",
    "SEMANTIC_OPTIMIZATION_PIPELINE_VALIDATION_TIER",
    "action_validate_optimization_proof_model",
    "action_validate_semantic_optimization_pipeline",
]
