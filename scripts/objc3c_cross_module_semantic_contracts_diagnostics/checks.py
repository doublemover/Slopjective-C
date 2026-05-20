from __future__ import annotations

from typing import Any

from objc3c_cross_module_semantic_contracts_diagnostics.catalog import CONFORMANCE_NEGATIVE
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import CONFORMANCE_POSITIVE
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import FRONTEND_ARTIFACTS
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import FRONTEND_PIPELINE
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import FRONTEND_TYPES
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import IR_EMITTER_H
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import LANDED_FLAGS
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import LOWERING_CONTRACT
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import NEGATIVE_FIXTURE
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import POSITIVE_FIXTURE
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import POSITIVE_MIN_COUNTS
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import REPLAY_SEGMENTS
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import SEMA_CONTRACT
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import SEMANTIC_MANIFEST
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import SEMANTIC_PASSES
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import SEMANTIC_PASSES_H
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import SEMANTIC_README
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import SOURCE_TRUTH_PATHS
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import STRESS_MANIFEST
from objc3c_cross_module_semantic_contracts_diagnostics.catalog import SUMMARY_FIELDS
from objc3c_cross_module_semantic_contracts_diagnostics.diagnostics import diagnostic_matches
from objc3c_cross_module_semantic_contracts_diagnostics.paths import read
from objc3c_cross_module_semantic_contracts_diagnostics.paths import rel
from objc3c_tooling.json_io import load_json_any as load_json
from objc3c_tooling.validation import contains_all


def compile_static_presence() -> dict[str, dict[str, bool]]:
    return {
        "sema_contract": contains_all(read(SEMA_CONTRACT), ["Objc3CrossModuleSemanticContractsDiagnosticsSummary", "kObjc3CrossModuleSemanticContractsDiagnosticsContractId"] + SUMMARY_FIELDS),
        "semantic_passes_header": contains_all(read(SEMANTIC_PASSES_H), ["BuildCrossModuleSemanticContractsDiagnosticsSummary"]),
        "semantic_passes_cpp": contains_all(read(SEMANTIC_PASSES), ["BuildCrossModuleSemanticContractsDiagnosticsSummary", "module_import_graph_summary", "namespace_collision_shadowing_summary", "public_private_api_partition_summary", "incremental_module_cache_invalidation_summary", "cross_module_conformance_summary", "error_diagnostics_recovery_summary"] + REPLAY_SEGMENTS[:-1] + ["violations="]),
        "frontend_types": contains_all(read(FRONTEND_TYPES), ["cross_module_semantic_contracts_diagnostics_summary"]),
        "frontend_pipeline": contains_all(read(FRONTEND_PIPELINE), ["BuildCrossModuleSemanticContractsDiagnosticsSummary", "cross_module_semantic_contracts_diagnostics_summary"]),
        "frontend_artifacts": contains_all(read(FRONTEND_ARTIFACTS), ["BuildCrossModuleSemanticContractsDiagnosticsSummaryJson", "objc_cross_module_semantic_contracts_and_diagnostics", "cross_module_semantic_contracts_diagnostics_summary"] + SUMMARY_FIELDS),
        "lowering_contract": contains_all(read(LOWERING_CONTRACT), ["kObjc3ModuleImportGraphLoweringLaneContract", "kObjc3NamespaceCollisionShadowingLoweringLaneContract", "kObjc3PublicPrivateApiPartitionLoweringLaneContract", "kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract", "kObjc3CrossModuleConformanceLoweringLaneContract", "kObjc3ErrorDiagnosticsRecoveryLoweringLaneContract"]),
        "ir_emitter_metadata": contains_all(read(IR_EMITTER_H), ["lowering_module_import_graph_replay_key", "lowering_namespace_collision_shadowing_replay_key", "lowering_public_private_api_partition_replay_key", "lowering_incremental_module_cache_invalidation_replay_key", "lowering_cross_module_conformance_replay_key", "lowering_error_diagnostics_recovery_replay_key"]),
    }


def compile_contract_checks(
    *,
    model: dict[str, Any] | None,
    replay_key: str,
    positive_run: dict[str, Any],
    negative_run: dict[str, Any],
    static_presence: dict[str, dict[str, bool]],
) -> dict[str, bool]:
    manifest_text = read(SEMANTIC_MANIFEST)
    readme_text = read(SEMANTIC_README)
    stress_manifest_text = read(STRESS_MANIFEST)
    conformance_positive = load_json(CONFORMANCE_POSITIVE)
    conformance_negative = load_json(CONFORMANCE_NEGATIVE)

    return {
        "positive_fixture_compiles": positive_run["exit_code"] == 0,
        "positive_manifest_emitted": positive_run["manifest_path"] is not None,
        "positive_llvm_ir_emitted": positive_run["llvm_ir_path"] is not None,
        "manifest_has_cross_module_surface": model is not None,
        "all_summary_fields_emitted": bool(model) and all(field in model for field in SUMMARY_FIELDS),
        "positive_minimum_counts_observed": bool(model) and all(int(model.get(field, -1)) >= minimum for field, minimum in POSITIVE_MIN_COUNTS.items()),
        "positive_landed_flags_true": bool(model) and all(bool(model.get(flag)) for flag in LANDED_FLAGS),
        "positive_contract_violations_zero": bool(model) and int(model.get("contract_violation_sites", -1)) == 0,
        "positive_ready_and_deterministic": bool(model) and bool(model.get("deterministic")) and bool(model.get("ready_for_lowering_and_runtime")),
        "replay_key_covers_cross_module_axes": all(segment in replay_key for segment in REPLAY_SEGMENTS),
        "negative_fixture_fails_closed": negative_run["exit_code"] != 0,
        "negative_diagnostics_json_emitted": negative_run["diagnostics_path"] is not None,
        "negative_duplicate_module_diagnostic_observed": diagnostic_matches(negative_run["diagnostics"], "O3S200", 3, 8),
        "semantic_manifest_indexes_xmod_8015_01": "XMOD-8015-01.json" in manifest_text,
        "semantic_manifest_indexes_xmod_8015_02": "XMOD-8015-02.json" in manifest_text,
        "semantic_readme_mentions_issue_8015": "#8015" in readme_text,
        "semantic_readme_mentions_positive_fixture": rel(POSITIVE_FIXTURE) in readme_text,
        "semantic_readme_mentions_negative_fixture": rel(NEGATIVE_FIXTURE) in readme_text,
        "positive_conformance_references_fixture": rel(POSITIVE_FIXTURE) in conformance_positive.get("references", []),
        "negative_conformance_references_fixture": rel(NEGATIVE_FIXTURE) in conformance_negative.get("references", []),
        "negative_conformance_expects_o3s200_location": conformance_negative.get("expect", {}).get("diagnostics") == [{"code": "O3S200", "line": 3, "column": 8}],
        "stress_manifest_tracks_positive_fixture_as_semantic_provenance": rel(POSITIVE_FIXTURE) in stress_manifest_text,
        "no_tmp_source_truth": all(not rel(path).startswith("tmp/") for path in SOURCE_TRUTH_PATHS),
        "static_sources_thread_surface": all(all(values.values()) for values in static_presence.values()),
    }
