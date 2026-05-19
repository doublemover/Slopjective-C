from __future__ import annotations

from objc3c_cross_module_semantic_contracts_diagnostics.paths import ROOT

CONTRACT_ID = "objc3c.cross_module.semantic.contracts.diagnostics.closure.v1"
ISSUE = "#8015"

POSITIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "cross_module_semantic_contracts_diagnostics_positive.objc3"
NEGATIVE_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "negative" / "negative_cross_module_semantic_contracts_duplicate_module.objc3"
SEMANTIC_MANIFEST = ROOT / "tests" / "conformance" / "semantic" / "manifest.json"
SEMANTIC_README = ROOT / "tests" / "conformance" / "semantic" / "README.md"
CONFORMANCE_POSITIVE = ROOT / "tests" / "conformance" / "semantic" / "XMOD-8015-01.json"
CONFORMANCE_NEGATIVE = ROOT / "tests" / "conformance" / "semantic" / "XMOD-8015-02.json"
STRESS_MANIFEST = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "lowering_runtime_stress_manifest.json"
SEMA_CONTRACT = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_contract.h"
SEMANTIC_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
SEMANTIC_PASSES_H = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.h"
FRONTEND_TYPES = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
FRONTEND_PIPELINE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp"
FRONTEND_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "artifacts" / "objc3_frontend_artifacts.cpp"
LOWERING_CONTRACT = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER_H = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.h"

SUMMARY_FIELDS = [
    "module_import_graph_sites",
    "import_edge_candidate_sites",
    "namespace_segment_sites",
    "object_pointer_type_sites",
    "pointer_declarator_sites",
    "namespace_collision_shadowing_sites",
    "public_private_api_partition_sites",
    "incremental_module_cache_invalidation_sites",
    "cross_module_conformance_sites",
    "normalized_cross_module_sites",
    "cache_invalidation_candidate_sites",
    "diagnostic_recovery_sites",
    "diagnostic_emit_sites",
    "recovery_anchor_sites",
    "recovery_boundary_sites",
    "fail_closed_diagnostic_sites",
    "diagnostic_normalized_sites",
    "diagnostic_gate_blocked_sites",
    "interop_import_module_annotation_sites",
    "interop_imported_module_name_sites",
    "contract_violation_sites",
    "module_import_graph_semantics_landed",
    "namespace_collision_semantics_landed",
    "public_private_partition_semantics_landed",
    "incremental_cache_semantics_landed",
    "cross_module_conformance_semantics_landed",
    "diagnostic_recovery_semantics_landed",
    "interop_import_semantics_landed",
    "deterministic",
    "ready_for_lowering_and_runtime",
    "replay_key",
]

POSITIVE_MIN_COUNTS = {
    "module_import_graph_sites": 4,
    "import_edge_candidate_sites": 4,
    "namespace_segment_sites": 4,
    "object_pointer_type_sites": 7,
    "pointer_declarator_sites": 4,
    "namespace_collision_shadowing_sites": 4,
    "public_private_api_partition_sites": 4,
    "incremental_module_cache_invalidation_sites": 4,
    "cross_module_conformance_sites": 4,
    "normalized_cross_module_sites": 4,
    "interop_import_module_annotation_sites": 1,
    "interop_imported_module_name_sites": 1,
}

LANDED_FLAGS = [
    "module_import_graph_semantics_landed",
    "namespace_collision_semantics_landed",
    "public_private_partition_semantics_landed",
    "incremental_cache_semantics_landed",
    "cross_module_conformance_semantics_landed",
    "diagnostic_recovery_semantics_landed",
    "interop_import_semantics_landed",
]

REPLAY_SEGMENTS = [
    "module-import=",
    "namespace=",
    "api-partition=",
    "incremental-cache=",
    "cross-module=",
    "diagnostics=",
    "interop-import=",
    "violations=0",
]

SOURCE_TRUTH_PATHS = [
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
]

VALIDATION_COMMANDS = [
    "python scripts/build_objc3c_cross_module_semantic_contracts_diagnostics.py --check",
    "python -m pytest tests/tooling/test_build_objc3c_cross_module_semantic_contracts_diagnostics.py",
    "npm run objc3c -- test-negative-expectations",
    "npm run objc3c -- test-execution-replay",
    "npm run objc3c -- test-lowering-runtime-stress",
    "npm run objc3c -- test-full",
]
