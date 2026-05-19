from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_any as load_json
from objc3c_tooling.validation import contains_all

from objc3c_effects_ownership_semantic_model.contracts import CONTRACT_TOKENS
from objc3c_effects_ownership_semantic_model.contracts import FRONTEND_ARTIFACT_TOKENS
from objc3c_effects_ownership_semantic_model.contracts import IR_METADATA_TOKENS
from objc3c_effects_ownership_semantic_model.contracts import LOWERING_CONTRACT_TOKENS
from objc3c_effects_ownership_semantic_model.contracts import SEMANTIC_PASS_TOKENS
from objc3c_effects_ownership_semantic_model.contracts import SOURCE_REPLAY_SEGMENTS
from objc3c_effects_ownership_semantic_model.contracts import SUMMARY_FIELDS
from objc3c_effects_ownership_semantic_model.paths import CONFORMANCE_NEGATIVE
from objc3c_effects_ownership_semantic_model.paths import CONFORMANCE_POSITIVE
from objc3c_effects_ownership_semantic_model.paths import FRONTEND_ARTIFACTS
from objc3c_effects_ownership_semantic_model.paths import FRONTEND_PIPELINE
from objc3c_effects_ownership_semantic_model.paths import FRONTEND_TYPES
from objc3c_effects_ownership_semantic_model.paths import IR_EMITTER
from objc3c_effects_ownership_semantic_model.paths import IR_EMITTER_H
from objc3c_effects_ownership_semantic_model.paths import LOWERING_CONTRACT
from objc3c_effects_ownership_semantic_model.paths import NEGATIVE_FIXTURE
from objc3c_effects_ownership_semantic_model.paths import POSITIVE_FIXTURE
from objc3c_effects_ownership_semantic_model.paths import SEMA_CONTRACT
from objc3c_effects_ownership_semantic_model.paths import SEMANTIC_MANIFEST
from objc3c_effects_ownership_semantic_model.paths import SEMANTIC_PASSES
from objc3c_effects_ownership_semantic_model.paths import SEMANTIC_PASSES_H
from objc3c_effects_ownership_semantic_model.paths import SEMANTIC_README
from objc3c_effects_ownership_semantic_model.paths import STRESS_MANIFEST
from objc3c_effects_ownership_semantic_model.paths import read


def load_semantic_inputs() -> dict[str, Any]:
    return {
        "manifest_text": read(SEMANTIC_MANIFEST),
        "readme_text": read(SEMANTIC_README),
        "stress_manifest_text": read(STRESS_MANIFEST),
        "conformance_positive": load_json(CONFORMANCE_POSITIVE),
        "conformance_negative": load_json(CONFORMANCE_NEGATIVE),
    }


def build_static_presence() -> dict[str, dict[str, bool]]:
    return {
        "sema_contract": contains_all(read(SEMA_CONTRACT), CONTRACT_TOKENS + SUMMARY_FIELDS),
        "semantic_passes_header": contains_all(read(SEMANTIC_PASSES_H), ["BuildEffectsOwnershipSemanticModelSummary"]),
        "semantic_passes_cpp": contains_all(read(SEMANTIC_PASSES), SEMANTIC_PASS_TOKENS + SOURCE_REPLAY_SEGMENTS),
        "frontend_types": contains_all(read(FRONTEND_TYPES), ["effects_ownership_semantic_model_summary"]),
        "frontend_pipeline": contains_all(
            read(FRONTEND_PIPELINE),
            ["BuildEffectsOwnershipSemanticModelSummary", "effects_ownership_semantic_model_summary"],
        ),
        "frontend_artifacts": contains_all(read(FRONTEND_ARTIFACTS), FRONTEND_ARTIFACT_TOKENS + SUMMARY_FIELDS),
        "lowering_contract": contains_all(read(LOWERING_CONTRACT), LOWERING_CONTRACT_TOKENS),
        "ir_emitter_metadata": contains_all(read(IR_EMITTER_H) + read(IR_EMITTER), IR_METADATA_TOKENS),
    }


def source_truth_paths() -> list[Path]:
    return [
        POSITIVE_FIXTURE,
        NEGATIVE_FIXTURE,
        CONFORMANCE_POSITIVE,
        CONFORMANCE_NEGATIVE,
        SEMANTIC_MANIFEST,
        SEMANTIC_README,
        STRESS_MANIFEST,
        SEMA_CONTRACT,
        SEMANTIC_PASSES_H,
        SEMANTIC_PASSES,
        FRONTEND_TYPES,
        FRONTEND_PIPELINE,
        FRONTEND_ARTIFACTS,
        LOWERING_CONTRACT,
        IR_EMITTER_H,
        IR_EMITTER,
    ]
