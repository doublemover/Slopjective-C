from __future__ import annotations

from objc3c_effects_ownership_semantic_model.cli import main
from objc3c_effects_ownership_semantic_model.compiler import find_effects_model
from objc3c_effects_ownership_semantic_model.compiler import run_compiler
from objc3c_effects_ownership_semantic_model.contracts import CONTRACT_ID
from objc3c_effects_ownership_semantic_model.contracts import CONTRACT_TOKENS
from objc3c_effects_ownership_semantic_model.contracts import FRONTEND_ARTIFACT_TOKENS
from objc3c_effects_ownership_semantic_model.contracts import IR_METADATA_TOKENS
from objc3c_effects_ownership_semantic_model.contracts import ISSUE
from objc3c_effects_ownership_semantic_model.contracts import LANDED_FLAGS
from objc3c_effects_ownership_semantic_model.contracts import LOWERING_CONTRACT_TOKENS
from objc3c_effects_ownership_semantic_model.contracts import POSITIVE_MIN_COUNTS
from objc3c_effects_ownership_semantic_model.contracts import RUNTIME_REPLAY_SEGMENTS
from objc3c_effects_ownership_semantic_model.contracts import SEMANTIC_PASS_TOKENS
from objc3c_effects_ownership_semantic_model.contracts import SOURCE_REPLAY_SEGMENTS
from objc3c_effects_ownership_semantic_model.contracts import SUMMARY_FIELDS
from objc3c_effects_ownership_semantic_model.contracts import VALIDATION_COMMANDS
from objc3c_effects_ownership_semantic_model.inputs import build_static_presence
from objc3c_effects_ownership_semantic_model.inputs import load_json
from objc3c_effects_ownership_semantic_model.inputs import load_semantic_inputs
from objc3c_effects_ownership_semantic_model.inputs import source_truth_paths
from objc3c_effects_ownership_semantic_model.paths import COMPILER
from objc3c_effects_ownership_semantic_model.paths import CONFORMANCE_NEGATIVE
from objc3c_effects_ownership_semantic_model.paths import CONFORMANCE_POSITIVE
from objc3c_effects_ownership_semantic_model.paths import FRONTEND_ARTIFACTS
from objc3c_effects_ownership_semantic_model.paths import FRONTEND_PIPELINE
from objc3c_effects_ownership_semantic_model.paths import FRONTEND_TYPES
from objc3c_effects_ownership_semantic_model.paths import IR_EMITTER
from objc3c_effects_ownership_semantic_model.paths import IR_EMITTER_H
from objc3c_effects_ownership_semantic_model.paths import JSON_OUT
from objc3c_effects_ownership_semantic_model.paths import LOWERING_CONTRACT
from objc3c_effects_ownership_semantic_model.paths import MD_OUT
from objc3c_effects_ownership_semantic_model.paths import NEGATIVE_FIXTURE
from objc3c_effects_ownership_semantic_model.paths import POSITIVE_FIXTURE
from objc3c_effects_ownership_semantic_model.paths import REPORT_DIR
from objc3c_effects_ownership_semantic_model.paths import ROOT
from objc3c_effects_ownership_semantic_model.paths import SEMA_CONTRACT
from objc3c_effects_ownership_semantic_model.paths import SEMANTIC_MANIFEST
from objc3c_effects_ownership_semantic_model.paths import SEMANTIC_PASSES
from objc3c_effects_ownership_semantic_model.paths import SEMANTIC_PASSES_H
from objc3c_effects_ownership_semantic_model.paths import SEMANTIC_README
from objc3c_effects_ownership_semantic_model.paths import STRESS_MANIFEST
from objc3c_effects_ownership_semantic_model.paths import TMP_ROOT
from objc3c_effects_ownership_semantic_model.paths import read
from objc3c_effects_ownership_semantic_model.paths import rel
from objc3c_effects_ownership_semantic_model.rendering import render_markdown
from objc3c_effects_ownership_semantic_model.rendering import write_outputs
from objc3c_effects_ownership_semantic_model.semantic_model import build_summary
from objc3c_effects_ownership_semantic_model.validation import build_checks
from objc3c_effects_ownership_semantic_model.validation import diagnostic_matches

__all__ = [
    "COMPILER",
    "CONFORMANCE_NEGATIVE",
    "CONFORMANCE_POSITIVE",
    "CONTRACT_ID",
    "CONTRACT_TOKENS",
    "FRONTEND_ARTIFACTS",
    "FRONTEND_ARTIFACT_TOKENS",
    "FRONTEND_PIPELINE",
    "FRONTEND_TYPES",
    "IR_EMITTER",
    "IR_EMITTER_H",
    "IR_METADATA_TOKENS",
    "ISSUE",
    "JSON_OUT",
    "LANDED_FLAGS",
    "LOWERING_CONTRACT",
    "LOWERING_CONTRACT_TOKENS",
    "MD_OUT",
    "NEGATIVE_FIXTURE",
    "POSITIVE_FIXTURE",
    "POSITIVE_MIN_COUNTS",
    "REPORT_DIR",
    "ROOT",
    "RUNTIME_REPLAY_SEGMENTS",
    "SEMA_CONTRACT",
    "SEMANTIC_MANIFEST",
    "SEMANTIC_PASSES",
    "SEMANTIC_PASSES_H",
    "SEMANTIC_PASS_TOKENS",
    "SEMANTIC_README",
    "SOURCE_REPLAY_SEGMENTS",
    "STRESS_MANIFEST",
    "SUMMARY_FIELDS",
    "TMP_ROOT",
    "VALIDATION_COMMANDS",
    "build_checks",
    "build_static_presence",
    "build_summary",
    "diagnostic_matches",
    "find_effects_model",
    "load_json",
    "load_semantic_inputs",
    "main",
    "read",
    "rel",
    "render_markdown",
    "run_compiler",
    "source_truth_paths",
    "write_outputs",
]


if __name__ == "__main__":
    raise SystemExit(main())
