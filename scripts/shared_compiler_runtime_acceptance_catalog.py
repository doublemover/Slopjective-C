"""Catalog metadata for the shared executable runtime acceptance harness."""

from __future__ import annotations

from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any, Sequence

from objc3c_runtime_acceptance.compile_truth import (
    COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID,
    COMPILE_PROVENANCE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.domains.metaprogramming_lowering_surfaces import (
    RUNTIME_CROSS_MODULE_METAPROGRAMMING_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_METAPROGRAMMING_LOWERING_HOST_CACHE_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.domains.metaprogramming_runtime_cache_surfaces import (
    RUNTIME_METAPROGRAMMING_CACHE_RUNTIME_INTEGRATION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_METAPROGRAMMING_RUNTIME_ABI_CACHE_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.domains.metaprogramming_semantic_surfaces import (
    RUNTIME_METAPROGRAMMING_SEMANTICS_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.domains.metaprogramming_source_surfaces import (
    RUNTIME_METAPROGRAMMING_PACKAGE_PROVENANCE_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_METAPROGRAMMING_SOURCE_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.domains.probe_helpers import (
    RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.runtime_contract_block_arc import (
    RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
    RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.runtime_contract_errors import (
    RUNTIME_BRIDGING_FILTER_UNWIND_DIAGNOSTICS_SURFACE_CONTRACT_ID,
    RUNTIME_CATCH_FILTER_FINALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_EXECUTION_CLEANUP_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_PROPAGATION_CATCH_CLEANUP_RUNTIME_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_PROPAGATION_CLEANUP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_RUNTIME_ABI_CLEANUP_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.runtime_contract_interop import (
    RUNTIME_C_CPP_SWIFT_BRIDGE_COMPATIBILITY_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_LANGUAGE_REPLAY_IMPORT_SURFACE_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_IMPORT_VERSION_FEATURE_CLAIM_DIAGNOSTICS_SURFACE_CONTRACT_ID,
    RUNTIME_MIXED_IMAGE_COMPATIBILITY_INTEROP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADER_BRIDGE_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADING_INTEROP_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADING_MODULE_IDENTITY_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.runtime_contract_object_model import (
    RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
    RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.runtime_contract_registration import (
    RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.runtime_contract_release import (
    RUNTIME_CLAIM_PUBLICATION_DASHBOARD_SCHEMA_SURFACE_CONTRACT_ID,
    RUNTIME_CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SURFACE_CONTRACT_ID,
    RUNTIME_CLAIMABLE_SURFACE_RESIDUAL_NON_CLAIMABLE_GAPS_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_FINAL_CLAIM_PUBLICATION_DEPRECATED_PATH_SHUTDOWN_SURFACE_CONTRACT_ID,
    RUNTIME_FINAL_RELEASE_EVIDENCE_DESCAFFOLDING_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_SCAFFOLD_RETIREMENT_DEPRECATED_SIDECAR_COMPATIBILITY_DIAGNOSTICS_SURFACE_CONTRACT_ID,
    RUNTIME_STRICT_PROFILE_CLAIM_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_STRICT_PROFILE_FEATURE_CLAIM_SOURCE_SURFACE_CONTRACT_ID,
)
from objc3c_runtime_acceptance.runtime_contract_storage_reflection import (
    RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
)
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.public_runner import public_workflow_command_tuple
from objc3c_tooling.subprocesses import python_script_command_tuple


ROOT = Path(__file__).resolve().parents[1]
HARNESS_REPORT_ROOT = (
    ROOT / "tmp" / "reports" / "runtime" / "shared-executable-acceptance-harness"
)
HARNESS_CONTRACT_ID = "objc3c.shared.executable.acceptance.harness.v1"
HARNESS_SUMMARY_CONTRACT_ID = "objc3c.shared.executable.acceptance.harness.summary.v1"
CLAIM_BOUNDARY_CONTRACT_ID = "objc3c.runtime.execution.claim.boundary.v1"


@dataclass(frozen=True)
class SurfaceRequirement:
    key: str
    contract_id: str
    required_fields: tuple[str, ...] = ()


@dataclass(frozen=True)
class SuiteEntry:
    suite_id: str
    summary: str
    execution_kind: str
    command: tuple[str, ...]
    report_path: str
    guarantee_owner: str
    validation_owner: str
    required_surfaces: tuple[SurfaceRequirement, ...]


COMMON_SURFACES = (
    SurfaceRequirement("claim_boundary", CLAIM_BOUNDARY_CONTRACT_ID),
    SurfaceRequirement(
        "runtime_state_publication_surface",
        RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
        ("publication_surface_kind", "compile_artifact_set", "public_runtime_abi_boundary"),
    ),
    SurfaceRequirement(
        "acceptance_suite_surface",
        RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID,
        (
            "suite_path",
            "report_path",
            "authoritative_claim_classes",
            "compile_output_provenance_contract_id",
            "compile_output_truthfulness_contract_id",
        ),
    ),
    SurfaceRequirement(
        "runtime_error_execution_cleanup_source_surface",
        RUNTIME_ERROR_EXECUTION_CLEANUP_SOURCE_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "authoritative_code_paths", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_catch_filter_finalization_source_surface",
        RUNTIME_CATCH_FILTER_FINALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "authoritative_code_paths", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_error_propagation_cleanup_semantics_surface",
        RUNTIME_ERROR_PROPAGATION_CLEANUP_SEMANTICS_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "authoritative_code_paths", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_bridging_filter_unwind_diagnostics_surface",
        RUNTIME_BRIDGING_FILTER_UNWIND_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "authoritative_code_paths", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_error_lowering_unwind_bridge_helper_surface",
        RUNTIME_ERROR_LOWERING_UNWIND_BRIDGE_HELPER_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "authoritative_code_paths", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_error_runtime_abi_cleanup_surface",
        RUNTIME_ERROR_RUNTIME_ABI_CLEANUP_SURFACE_CONTRACT_ID,
        ("authoritative_case_ids", "authoritative_probe_path", "public_runtime_abi_boundary"),
    ),
    SurfaceRequirement(
        "runtime_error_propagation_catch_cleanup_runtime_implementation_surface",
        RUNTIME_ERROR_PROPAGATION_CATCH_CLEANUP_RUNTIME_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        (
            "authoritative_case_ids",
            "authoritative_probe_paths",
            "private_error_runtime_abi_boundary",
        ),
    ),
    SurfaceRequirement(
        "runtime_object_model_realization_source_surface",
        RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "private_object_model_query_boundary", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_block_arc_unified_source_surface",
        RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "authoritative_code_paths", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_ownership_transfer_capture_family_source_surface",
        RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
        ("block_capture_ownership_contract_id", "authoritative_code_paths", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_block_arc_lowering_helper_surface",
        RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
        ("runtime_block_arc_runtime_abi_surface_contract_id", "semantic_surface_paths", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_block_arc_runtime_abi_surface",
        RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        ("block_arc_runtime_abi_snapshot_symbol", "arc_debug_state_snapshot_symbol", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_property_ivar_storage_accessor_source_surface",
        RUNTIME_PROPERTY_IVAR_STORAGE_ACCESSOR_SOURCE_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "authoritative_code_paths", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "storage_accessor_runtime_abi_surface",
        RUNTIME_STORAGE_ACCESSOR_RUNTIME_ABI_SURFACE_CONTRACT_ID,
        ("property_registry_state_snapshot_symbol", "current_property_read_symbol", "weak_current_property_store_symbol"),
    ),
    SurfaceRequirement(
        "runtime_property_ivar_accessor_reflection_implementation_surface",
        RUNTIME_PROPERTY_IVAR_ACCESSOR_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        ("implementation_snapshot_symbol", "property_registry_state_snapshot_symbol", "property_entry_snapshot_symbol"),
    ),
    SurfaceRequirement(
        "runtime_claimable_surface_residual_non_claimable_gaps_source_surface",
        RUNTIME_CLAIMABLE_SURFACE_RESIDUAL_NON_CLAIMABLE_GAPS_SOURCE_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "authoritative_source_fields", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_strict_profile_feature_claim_source_surface",
        RUNTIME_STRICT_PROFILE_FEATURE_CLAIM_SOURCE_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "authoritative_source_fields", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_claimability_semantics_release_policy_surface",
        RUNTIME_CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "policy_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_strict_profile_claim_implementation_surface",
        RUNTIME_STRICT_PROFILE_CLAIM_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "claim_implementation_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_surface",
        RUNTIME_SCAFFOLD_RETIREMENT_DEPRECATED_SIDECAR_COMPATIBILITY_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "compatibility_diagnostic_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_claim_publication_dashboard_schema_surface",
        RUNTIME_CLAIM_PUBLICATION_DASHBOARD_SCHEMA_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "dashboard_schema_path", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_final_claim_publication_deprecated_path_shutdown_surface",
        RUNTIME_FINAL_CLAIM_PUBLICATION_DEPRECATED_PATH_SHUTDOWN_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "final_publication_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_release_candidate_claim_abi_surface",
        RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
        (
            "release_candidate_claim_snapshot_symbol",
            "release_candidate_claim_snapshot_type",
            "authoritative_probe_paths",
        ),
    ),
    SurfaceRequirement(
        "runtime_final_release_evidence_descaffolding_implementation_surface",
        RUNTIME_FINAL_RELEASE_EVIDENCE_DESCAFFOLDING_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        ("release_candidate_evidence_snapshot_symbol", "implementation_model", "authoritative_probe_paths"),
    ),
    SurfaceRequirement(
        "runtime_metaprogramming_source_surface",
        RUNTIME_METAPROGRAMMING_SOURCE_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "authoritative_code_paths", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_metaprogramming_package_provenance_source_surface",
        RUNTIME_METAPROGRAMMING_PACKAGE_PROVENANCE_SOURCE_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "authoritative_code_paths", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_metaprogramming_semantics_surface",
        RUNTIME_METAPROGRAMMING_SEMANTICS_SURFACE_CONTRACT_ID,
        ("semantic_contract_ids", "authoritative_code_paths", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_metaprogramming_lowering_host_cache_surface",
        RUNTIME_METAPROGRAMMING_LOWERING_HOST_CACHE_SURFACE_CONTRACT_ID,
        ("host_cache_artifact", "expansion_lowering_contract_id", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_cross_module_metaprogramming_artifact_preservation_surface",
        RUNTIME_CROSS_MODULE_METAPROGRAMMING_ARTIFACT_PRESERVATION_SURFACE_CONTRACT_ID,
        ("runtime_import_surface_artifact", "cross_module_link_plan_artifact", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_metaprogramming_runtime_abi_cache_surface",
        RUNTIME_METAPROGRAMMING_RUNTIME_ABI_CACHE_SURFACE_CONTRACT_ID,
        ("macro_host_process_cache_integration_snapshot_symbol", "authoritative_probe_paths", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_metaprogramming_cache_runtime_integration_implementation_surface",
        RUNTIME_METAPROGRAMMING_CACHE_RUNTIME_INTEGRATION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        ("authoritative_probe_paths", "authoritative_case_ids", "macro_host_process_cache_integration_snapshot_symbol"),
    ),
    SurfaceRequirement(
        "runtime_cross_module_package_interop_source_surface",
        RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "authoritative_code_paths", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_textual_binary_interface_parity_source_surface",
        RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "authoritative_source_fields", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_mixed_image_compatibility_interop_semantics_surface",
        RUNTIME_MIXED_IMAGE_COMPATIBILITY_INTEROP_SEMANTICS_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "language_profile_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_package_loading_module_identity_semantics_surface",
        RUNTIME_PACKAGE_LOADING_MODULE_IDENTITY_SEMANTICS_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "runtime_probe", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_c_cpp_swift_bridge_compatibility_semantics_surface",
        RUNTIME_C_CPP_SWIFT_BRIDGE_COMPATIBILITY_SEMANTICS_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "language_profile_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_import_version_feature_claim_diagnostics_surface",
        RUNTIME_IMPORT_VERSION_FEATURE_CLAIM_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "diagnostic_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_packaging_bridge_loader_artifact_surface",
        RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "artifact_surface_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_mixed_image_package_lowering_bridge_emission_surface",
        RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "lowering_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_cross_language_replay_import_surface_preservation_surface",
        RUNTIME_CROSS_LANGUAGE_REPLAY_IMPORT_SURFACE_PRESERVATION_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "preservation_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_package_loader_bridge_abi_surface",
        RUNTIME_PACKAGE_LOADER_BRIDGE_ABI_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "runtime_abi_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_package_loading_interop_implementation_surface",
        RUNTIME_PACKAGE_LOADING_INTEROP_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "implementation_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_property_atomicity_synthesis_reflection_source_surface",
        RUNTIME_PROPERTY_ATOMICITY_SYNTHESIS_REFLECTION_SOURCE_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "authoritative_code_paths", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_realization_lowering_reflection_artifact_surface",
        RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "lowering_artifact_boundary_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_dispatch_table_reflection_record_lowering_surface",
        RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "dispatch_table_lowering_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_cross_module_realized_metadata_replay_preservation_surface",
        RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "realized_metadata_replay_preservation_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_object_model_abi_query_surface",
        RUNTIME_OBJECT_MODEL_ABI_QUERY_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "private_object_model_query_boundary", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_realization_lookup_reflection_implementation_surface",
        RUNTIME_REALIZATION_LOOKUP_REFLECTION_IMPLEMENTATION_SURFACE_CONTRACT_ID,
        ("source_contract_ids", "object_model_query_state_snapshot_symbol", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_reflection_query_surface",
        RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
        ("query_api_boundary_model", "private_query_symbols", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_realization_lookup_semantics_surface",
        RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
        ("private_lookup_query_boundary", "lookup_resolution_order_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_class_metaclass_protocol_realization_surface",
        RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
        ("private_realization_query_boundary", "metaclass_lineage_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_category_attachment_merged_dispatch_surface",
        RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
        ("private_category_query_boundary", "merged_dispatch_resolution_model", "authoritative_case_ids"),
    ),
    SurfaceRequirement(
        "runtime_reflection_visibility_coherence_diagnostics_surface",
        RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
        ("private_coherence_query_boundary", "runtime_coherence_diagnostic_model", "authoritative_case_ids"),
    ),
)


SUITES: tuple[SuiteEntry, ...] = (
    SuiteEntry(
        suite_id="runtime-acceptance",
        summary="direct runtime acceptance suite over the live compile and runtime path",
        execution_kind="direct-executable-suite",
        command=python_script_command_tuple("scripts/check_objc3c_runtime_acceptance.py"),
        report_path="tmp/reports/runtime/acceptance/summary.json",
        guarantee_owner="runtime acceptance and compile-coupled runtime publication proof",
        validation_owner="scripts/check_objc3c_runtime_acceptance.py",
        required_surfaces=COMMON_SURFACES,
    ),
    SuiteEntry(
        suite_id="public-test-smoke",
        summary="composite smoke public workflow carrying runtime acceptance surfaces forward",
        execution_kind="composite-executable-suite",
        command=public_workflow_command_tuple("test-smoke"),
        report_path="tmp/reports/objc3c-public-workflow/test-smoke.json",
        guarantee_owner="behavior smoke, runtime acceptance, and replay through the public entrypoint",
        validation_owner="scripts.objc3c_workflow",
        required_surfaces=COMMON_SURFACES,
    ),
    SuiteEntry(
        suite_id="public-test-full",
        summary="composite full developer workflow carrying runtime acceptance surfaces forward",
        execution_kind="composite-executable-suite",
        command=public_workflow_command_tuple("test-full"),
        report_path="tmp/reports/objc3c-public-workflow/test-full.json",
        guarantee_owner="full developer validation without recovery fan-out",
        validation_owner="scripts.objc3c_workflow",
        required_surfaces=COMMON_SURFACES,
    ),
)


SUITE_MAP = {entry.suite_id: entry for entry in SUITES}


AUTHORITATIVE_CHILD_REPORT_CONTRACTS = [
    CLAIM_BOUNDARY_CONTRACT_ID,
    RUNTIME_STATE_PUBLICATION_SURFACE_CONTRACT_ID,
    RUNTIME_ACCEPTANCE_SUITE_SURFACE_CONTRACT_ID,
    RUNTIME_ERROR_EXECUTION_CLEANUP_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_OBJECT_MODEL_REALIZATION_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_BLOCK_ARC_UNIFIED_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_OWNERSHIP_TRANSFER_CAPTURE_FAMILY_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_BLOCK_ARC_LOWERING_HELPER_SURFACE_CONTRACT_ID,
    RUNTIME_BLOCK_ARC_RUNTIME_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_CLAIMABLE_SURFACE_RESIDUAL_NON_CLAIMABLE_GAPS_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_STRICT_PROFILE_FEATURE_CLAIM_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_CLAIMABILITY_SEMANTICS_RELEASE_POLICY_SURFACE_CONTRACT_ID,
    RUNTIME_STRICT_PROFILE_CLAIM_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_SCAFFOLD_RETIREMENT_DEPRECATED_SIDECAR_COMPATIBILITY_DIAGNOSTICS_SURFACE_CONTRACT_ID,
    RUNTIME_CLAIM_PUBLICATION_DASHBOARD_SCHEMA_SURFACE_CONTRACT_ID,
    RUNTIME_FINAL_CLAIM_PUBLICATION_DEPRECATED_PATH_SHUTDOWN_SURFACE_CONTRACT_ID,
    RUNTIME_RELEASE_CANDIDATE_CLAIM_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_FINAL_RELEASE_EVIDENCE_DESCAFFOLDING_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_PACKAGE_INTEROP_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_TEXTUAL_BINARY_INTERFACE_PARITY_SOURCE_SURFACE_CONTRACT_ID,
    RUNTIME_MIXED_IMAGE_COMPATIBILITY_INTEROP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADING_MODULE_IDENTITY_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_C_CPP_SWIFT_BRIDGE_COMPATIBILITY_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_IMPORT_VERSION_FEATURE_CLAIM_DIAGNOSTICS_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGING_BRIDGE_LOADER_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_MIXED_IMAGE_PACKAGE_LOWERING_BRIDGE_EMISSION_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_LANGUAGE_REPLAY_IMPORT_SURFACE_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADER_BRIDGE_ABI_SURFACE_CONTRACT_ID,
    RUNTIME_PACKAGE_LOADING_INTEROP_IMPLEMENTATION_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOWERING_REFLECTION_ARTIFACT_SURFACE_CONTRACT_ID,
    RUNTIME_DISPATCH_TABLE_REFLECTION_RECORD_LOWERING_SURFACE_CONTRACT_ID,
    RUNTIME_CROSS_MODULE_REALIZED_METADATA_REPLAY_PRESERVATION_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_QUERY_SURFACE_CONTRACT_ID,
    RUNTIME_REALIZATION_LOOKUP_SEMANTICS_SURFACE_CONTRACT_ID,
    RUNTIME_CLASS_METACLASS_PROTOCOL_REALIZATION_SURFACE_CONTRACT_ID,
    RUNTIME_CATEGORY_ATTACHMENT_MERGED_DISPATCH_SURFACE_CONTRACT_ID,
    RUNTIME_REFLECTION_VISIBILITY_COHERENCE_DIAGNOSTICS_SURFACE_CONTRACT_ID,
]


def build_harness_surface(selected: Sequence[SuiteEntry]) -> dict[str, Any]:
    return {
        "contract_id": HARNESS_CONTRACT_ID,
        "harness_path": "scripts/shared_compiler_runtime_acceptance_harness.py",
        "suite_ids": [entry.suite_id for entry in selected],
        "requires_real_executable_suite_reports": True,
        "authoritative_child_report_contracts": AUTHORITATIVE_CHILD_REPORT_CONTRACTS,
        "shared_compile_truth_contracts": [
            COMPILE_PROVENANCE_CONTRACT_ID,
            COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID,
        ],
    }


def build_catalog_payload(selected: Sequence[SuiteEntry]) -> dict[str, Any]:
    return {
        "contract_id": HARNESS_CONTRACT_ID,
        "harness_path": "scripts/shared_compiler_runtime_acceptance_harness.py",
        "suite_count": len(selected),
        "suites": [asdict(entry) for entry in selected],
    }


def check_catalog(selected: Sequence[SuiteEntry]) -> dict[str, Any]:
    suite_results: list[dict[str, Any]] = []
    ok = True
    for entry in selected:
        command_results = []
        for token in entry.command[1:]:
            candidate = ROOT / token
            if candidate.exists():
                command_results.append({"path": token, "exists": True})
            elif token.startswith("scripts/") or token.startswith("tests/"):
                command_results.append({"path": token, "exists": False})
                ok = False
        report_parent = (ROOT / entry.report_path).parent
        report_parent_exists = report_parent.exists()
        ok = ok and report_parent_exists
        suite_results.append(
            {
                "suite_id": entry.suite_id,
                "command_paths": command_results,
                "report_parent": repo_rel(report_parent),
                "report_parent_exists": report_parent_exists,
            }
        )
    return {"ok": ok, "suite_results": suite_results}


__all__ = [
    "COMPILE_OUTPUT_TRUTHFULNESS_CONTRACT_ID",
    "COMPILE_PROVENANCE_CONTRACT_ID",
    "HARNESS_CONTRACT_ID",
    "HARNESS_REPORT_ROOT",
    "HARNESS_SUMMARY_CONTRACT_ID",
    "SUITE_MAP",
    "SUITES",
    "SuiteEntry",
    "SurfaceRequirement",
    "build_catalog_payload",
    "build_harness_surface",
    "check_catalog",
]
