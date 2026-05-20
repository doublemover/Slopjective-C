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
from objc3c_effects_ownership_semantic_model.paths import CONFORMANCE_HELPER_SYMBOLS
from objc3c_effects_ownership_semantic_model.paths import CONFORMANCE_POSITIVE
from objc3c_effects_ownership_semantic_model.paths import MISSING_REQUIRED_SLICES_FIXTURE
from objc3c_effects_ownership_semantic_model.paths import NEGATIVE_FIXTURE
from objc3c_effects_ownership_semantic_model.paths import POSITIVE_FIXTURE
from objc3c_effects_ownership_semantic_model.paths import ROOT
from objc3c_effects_ownership_semantic_model.paths import SEMANTIC_MANIFEST
from objc3c_effects_ownership_semantic_model.paths import SEMANTIC_README
from objc3c_effects_ownership_semantic_model.paths import STRESS_MANIFEST
from objc3c_effects_ownership_semantic_model.paths import read


SEMA_SOURCE_ROOT = ROOT / "native" / "objc3c" / "src" / "sema"
FRONTEND_ARTIFACT_SOURCE_ROOT = ROOT / "native" / "objc3c" / "src" / "artifacts"
FRONTEND_PIPELINE_SOURCE_ROOT = ROOT / "native" / "objc3c" / "src" / "pipeline"
LOWERING_SOURCE_ROOT = ROOT / "native" / "objc3c" / "src" / "lower"
IR_SOURCE_ROOT = ROOT / "native" / "objc3c" / "src" / "ir"


def source_files_under(root: Path) -> list[Path]:
    return sorted(
        path
        for suffix in ("*.cpp", "*.h", "*.inc")
        for path in root.rglob(suffix)
    )


def read_sources(paths: list[Path]) -> str:
    return "\n".join(read(path) for path in paths)


def load_semantic_inputs() -> dict[str, Any]:
    return {
        "manifest_text": read(SEMANTIC_MANIFEST),
        "readme_text": read(SEMANTIC_README),
        "stress_manifest_text": read(STRESS_MANIFEST),
        "conformance_positive": load_json(CONFORMANCE_POSITIVE),
        "conformance_negative": load_json(CONFORMANCE_NEGATIVE),
        "conformance_helper_symbols": load_json(CONFORMANCE_HELPER_SYMBOLS),
    }


def build_static_presence() -> dict[str, dict[str, bool]]:
    sema_sources = source_files_under(SEMA_SOURCE_ROOT)
    frontend_artifact_sources = source_files_under(FRONTEND_ARTIFACT_SOURCE_ROOT)
    frontend_pipeline_sources = source_files_under(FRONTEND_PIPELINE_SOURCE_ROOT)
    lowering_sources = source_files_under(LOWERING_SOURCE_ROOT)
    ir_sources = source_files_under(IR_SOURCE_ROOT)
    sema_text = read_sources(sema_sources)
    frontend_artifact_text = read_sources(frontend_artifact_sources)
    frontend_pipeline_text = read_sources(frontend_pipeline_sources)
    lowering_text = read_sources(lowering_sources)
    ir_text = read_sources(ir_sources)
    return {
        "sema_contract": contains_all(sema_text, CONTRACT_TOKENS + SUMMARY_FIELDS),
        "semantic_passes_header": contains_all(sema_text, ["BuildEffectsOwnershipSemanticModelSummary"]),
        "semantic_passes_cpp": contains_all(sema_text, SEMANTIC_PASS_TOKENS + SOURCE_REPLAY_SEGMENTS),
        "frontend_types": contains_all(frontend_pipeline_text, ["effects_ownership_semantic_model_summary"]),
        "frontend_pipeline": contains_all(
            frontend_pipeline_text,
            ["BuildEffectsOwnershipSemanticModelSummary", "effects_ownership_semantic_model_summary"],
        ),
        "frontend_artifacts": contains_all(frontend_artifact_text, FRONTEND_ARTIFACT_TOKENS + SUMMARY_FIELDS),
        "lowering_contract": contains_all(
            lowering_text,
            LOWERING_CONTRACT_TOKENS,
        ),
        "ir_emitter_metadata": contains_all(ir_text + frontend_artifact_text, IR_METADATA_TOKENS),
    }


def source_truth_paths() -> list[Path]:
    return [
        POSITIVE_FIXTURE,
        MISSING_REQUIRED_SLICES_FIXTURE,
        NEGATIVE_FIXTURE,
        CONFORMANCE_POSITIVE,
        CONFORMANCE_NEGATIVE,
        CONFORMANCE_HELPER_SYMBOLS,
        SEMANTIC_MANIFEST,
        SEMANTIC_README,
        STRESS_MANIFEST,
        *source_files_under(SEMA_SOURCE_ROOT),
        *source_files_under(FRONTEND_ARTIFACT_SOURCE_ROOT),
        *source_files_under(FRONTEND_PIPELINE_SOURCE_ROOT),
        *source_files_under(LOWERING_SOURCE_ROOT),
        *source_files_under(IR_SOURCE_ROOT),
    ]
