"""Behavior-domain case factory sections for runtime acceptance."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any, Callable

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.probe_helpers import (
    check_runtime_probe_helper_support_case,
)

CaseFactory = Callable[[], CaseResult]
LabeledCaseFactories = list[tuple[str, CaseFactory]]


@dataclass(frozen=True)
class CaseFactoryContext:
    domains: Any
    clangxx: str
    run_dir: Path


def build_core_case_factories(context: CaseFactoryContext) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "runtime-library",
            lambda: domains.object_model.check_runtime_library_case(clangxx, run_dir),
        ),
        (
            "runtime-probe-helper-support",
            lambda: check_runtime_probe_helper_support_case(clangxx, run_dir),
        ),
        (
            "compile-backend-parity",
            lambda: domains.compiler_artifacts.check_compile_backend_parity_case(
                run_dir
            ),
        ),
        (
            "artifact-registry-key-isolation",
            lambda: domains.compiler_artifacts.check_artifact_registry_key_isolation_case(
                run_dir
            ),
        ),
    ]


def build_registration_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "installation-lifecycle",
            lambda: domains.registration.check_installation_lifecycle_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "multi-image-registration-reset-replay",
            lambda: domains.registration.check_multi_image_registration_reset_replay_case(
                clangxx,
                run_dir,
            ),
        ),
    ]


def build_metaprogramming_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "metaprogramming-source-surface",
            lambda: domains.metaprogramming.check_metaprogramming_source_surface_case(
                run_dir
            ),
        ),
        (
            "metaprogramming-package-provenance-source-surface",
            lambda: domains.metaprogramming.check_metaprogramming_package_provenance_source_surface_case(
                run_dir
            ),
        ),
        (
            "metaprogramming-semantics",
            lambda: domains.metaprogramming.check_metaprogramming_semantics_case(
                run_dir
            ),
        ),
        (
            "metaprogramming-derive-property-behavior-semantics",
            lambda: domains.metaprogramming.check_metaprogramming_derive_property_behavior_semantics_case(
                run_dir
            ),
        ),
        (
            "metaprogramming-macro-safety-cache-diagnostics",
            lambda: domains.metaprogramming.check_metaprogramming_macro_safety_cache_diagnostics_case(
                run_dir
            ),
        ),
        (
            "metaprogramming-lowering-host-cache-surface",
            lambda: domains.metaprogramming.check_metaprogramming_lowering_host_cache_surface_case(
                run_dir
            ),
        ),
        (
            "metaprogramming-executable-lowering",
            lambda: domains.metaprogramming.check_metaprogramming_executable_lowering_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "cross-module-metaprogramming-artifact-preservation",
            lambda: domains.metaprogramming.check_cross_module_metaprogramming_artifact_preservation_case(
                run_dir
            ),
        ),
        (
            "metaprogramming-runtime-abi-cache-surface",
            lambda: domains.metaprogramming.check_metaprogramming_runtime_abi_cache_surface_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "live-metaprogramming-cache-runtime-integration",
            lambda: domains.metaprogramming.check_live_metaprogramming_cache_runtime_integration_case(
                clangxx,
                run_dir,
            ),
        ),
    ]


def build_interop_packaging_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "cross-module-runtime-package-interop-source-surface",
            lambda: domains.interop_packaging.check_cross_module_runtime_package_interop_source_surface_case(
                run_dir
            ),
        ),
        (
            "textual-binary-interface-parity-source-surface",
            lambda: domains.interop_packaging.check_textual_binary_interface_parity_source_surface_case(
                run_dir
            ),
        ),
        (
            "mixed-image-compatibility-interop-semantics",
            lambda: domains.interop_packaging.check_mixed_image_compatibility_interop_semantics_case(
                run_dir
            ),
        ),
        (
            "c-cpp-swift-bridge-compatibility-semantics",
            lambda: domains.interop_packaging.check_c_cpp_swift_bridge_compatibility_semantics_case(
                run_dir
            ),
        ),
        (
            "import-version-feature-claim-diagnostics",
            lambda: domains.interop_packaging.check_import_version_feature_claim_diagnostics_case(
                run_dir
            ),
        ),
        (
            "runtime-packaging-bridge-loader-artifact-surface",
            lambda: domains.interop_packaging.check_runtime_packaging_bridge_loader_artifact_surface_case(
                run_dir
            ),
        ),
        (
            "mixed-image-package-lowering-bridge-emission",
            lambda: domains.interop_packaging.check_mixed_image_package_lowering_bridge_emission_case(
                run_dir
            ),
        ),
        (
            "cross-language-replay-import-surface-preservation",
            lambda: domains.interop_packaging.check_cross_language_replay_import_surface_preservation_case(
                run_dir
            ),
        ),
        (
            "runtime-package-loader-bridge-abi",
            lambda: domains.interop_packaging.check_runtime_package_loader_bridge_abi_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "live-package-loading-interop-runtime-implementation",
            lambda: domains.interop_packaging.check_live_package_loading_interop_runtime_implementation_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "imported-runtime-packaging-replay",
            lambda: domains.interop_packaging.check_imported_runtime_packaging_replay_case(
                clangxx,
                run_dir,
            ),
        ),
    ]


def build_release_claim_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "claimable-surface-residual-non-claimable-gaps-source-surface",
            lambda: domains.release_claims.check_claimable_surface_residual_non_claimable_gaps_source_surface_case(
                run_dir
            ),
        ),
        (
            "strict-profile-feature-claim-source-surface",
            lambda: domains.release_claims.check_strict_profile_feature_claim_source_surface_case(
                run_dir
            ),
        ),
        (
            "claimability-semantics-release-policy",
            lambda: domains.release_claims.check_claimability_semantics_release_policy_case(
                run_dir
            ),
        ),
        (
            "strict-profile-claim-implementation",
            lambda: domains.release_claims.check_strict_profile_claim_implementation_case(
                run_dir
            ),
        ),
        (
            "scaffold-retirement-deprecated-sidecar-compatibility-diagnostics",
            lambda: domains.release_claims.check_scaffold_retirement_deprecated_sidecar_compatibility_diagnostics_case(
                run_dir
            ),
        ),
        (
            "claim-publication-dashboard-schema-surface",
            lambda: domains.release_claims.check_claim_publication_dashboard_schema_surface_case(
                run_dir
            ),
        ),
        (
            "final-claim-publication-deprecated-path-shutdown",
            lambda: domains.release_claims.check_final_claim_publication_deprecated_path_shutdown_case(
                run_dir
            ),
        ),
        (
            "release-candidate-runtime-claim-abi",
            lambda: domains.release_claims.check_release_candidate_runtime_claim_abi_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "final-release-evidence-descaffolding-implementation",
            lambda: domains.release_claims.check_final_release_evidence_descaffolding_implementation_case(
                clangxx,
                run_dir,
            ),
        ),
    ]


def build_concurrency_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "unified-concurrency-runtime-architecture",
            lambda: domains.concurrency.check_unified_concurrency_runtime_architecture_case(
                run_dir
            ),
        ),
        (
            "async-task-actor-normalization-completion",
            lambda: domains.concurrency.check_async_task_actor_normalization_completion_case(
                run_dir
            ),
        ),
        (
            "unified-concurrency-lowering-metadata-surface",
            lambda: domains.concurrency.check_unified_concurrency_lowering_metadata_surface_case(
                run_dir
            ),
        ),
        (
            "unified-concurrency-runtime-abi",
            lambda: domains.concurrency.check_unified_concurrency_runtime_abi_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "live-unified-concurrency-runtime-implementation",
            lambda: domains.concurrency.check_live_unified_concurrency_runtime_implementation_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "cross-module-concurrency-actor-artifact-preservation",
            lambda: domains.concurrency.check_cross_module_concurrency_actor_artifact_preservation_case(
                run_dir
            ),
        ),
    ]


def build_error_case_factories(context: CaseFactoryContext) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "error-execution-cleanup-source",
            lambda: domains.errors.check_error_execution_cleanup_source_case(run_dir),
        ),
        (
            "catch-filter-finalization-source",
            lambda: domains.errors.check_catch_filter_finalization_source_case(run_dir),
        ),
        (
            "error-propagation-cleanup-semantics",
            lambda: domains.errors.check_error_propagation_cleanup_semantics_case(
                run_dir
            ),
        ),
        (
            "executable-try-throw-do-catch-semantics",
            lambda: domains.errors.check_executable_try_throw_do_catch_semantics_case(
                run_dir
            ),
        ),
        (
            "bridging-filter-unwind-compatibility-diagnostics",
            lambda: domains.errors.check_bridging_filter_unwind_compatibility_diagnostics_case(
                run_dir
            ),
        ),
        (
            "error-lowering-unwind-bridge-helper-surface",
            lambda: domains.errors.check_error_lowering_unwind_bridge_helper_surface_case(
                run_dir
            ),
        ),
        (
            "executable-throw-catch-cleanup-lowering",
            lambda: domains.errors.check_executable_throw_catch_cleanup_lowering_case(
                run_dir
            ),
        ),
        (
            "cross-module-error-metadata-replay-preservation",
            lambda: domains.errors.check_cross_module_error_metadata_replay_preservation_case(
                run_dir
            ),
        ),
        (
            "error-runtime-abi-cleanup",
            lambda: domains.errors.check_error_runtime_abi_cleanup_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "live-error-runtime-integration",
            lambda: domains.errors.check_live_error_runtime_integration_case(
                clangxx,
                run_dir,
            ),
        ),
    ]


def build_cross_module_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    run_dir = context.run_dir
    return [
        (
            "cross-module-block-ownership-artifact-preservation",
            lambda: domains.block_arc.check_cross_module_block_ownership_artifact_preservation_case(
                run_dir
            ),
        ),
        (
            "cross-module-storage-reflection-artifact-preservation",
            lambda: domains.storage_reflection.check_cross_module_storage_reflection_artifact_preservation_case(
                run_dir
            ),
        ),
    ]


def build_object_model_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "canonical-dispatch",
            lambda: domains.object_model.check_canonical_dispatch_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "metaclass-graph-root-class",
            lambda: domains.object_model.check_metaclass_graph_root_class_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "canonical-sample-set",
            lambda: domains.object_model.check_canonical_sample_set_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "realization-lookup-reflection-runtime",
            lambda: domains.object_model.check_realization_lookup_reflection_runtime_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "live-dispatch-fast-path",
            lambda: domains.object_model.check_live_dispatch_fast_path_case(
                clangxx,
                run_dir,
            ),
        ),
    ]


def build_storage_reflection_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "storage-ownership-reflection",
            lambda: domains.storage_reflection.check_storage_ownership_reflection_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "property-ivar-ordering-semantics",
            lambda: domains.storage_reflection.check_property_ivar_ordering_semantics_case(
                run_dir
            ),
        ),
        (
            "accessor-storage-lowering-metadata-surface",
            lambda: domains.storage_reflection.check_accessor_storage_lowering_metadata_surface_case(
                run_dir
            ),
        ),
        (
            "property-accessor-layout-lowering",
            lambda: domains.storage_reflection.check_property_accessor_layout_lowering_case(
                run_dir
            ),
        ),
        (
            "property-reflection-accessor-compatibility-diagnostics",
            lambda: domains.storage_reflection.check_property_reflection_accessor_compatibility_diagnostics_case(
                run_dir
            ),
        ),
        (
            "property-synthesis-storage-binding-semantics",
            lambda: domains.storage_reflection.check_property_synthesis_storage_binding_semantics_case(
                run_dir
            ),
        ),
        (
            "storage-legality-semantics",
            lambda: domains.storage_reflection.check_storage_legality_semantics_case(
                run_dir
            ),
        ),
        (
            "synthesized-accessor-codegen",
            lambda: domains.storage_reflection.check_synthesized_accessor_codegen_case(
                run_dir
            ),
        ),
        (
            "synthesized-accessor-runtime",
            lambda: domains.storage_reflection.check_synthesized_accessor_runtime_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "property-layout",
            lambda: domains.storage_reflection.check_property_layout_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "instance-allocation-layout-runtime",
            lambda: domains.storage_reflection.check_instance_allocation_layout_runtime_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "property-execution",
            lambda: domains.storage_reflection.check_property_execution_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "property-reflection",
            lambda: domains.storage_reflection.check_property_reflection_case(
                clangxx,
                run_dir,
            ),
        ),
    ]


def build_block_arc_case_factories(
    context: CaseFactoryContext,
) -> LabeledCaseFactories:
    domains = context.domains
    clangxx = context.clangxx
    run_dir = context.run_dir
    return [
        (
            "escaping-block-capture-legality",
            lambda: domains.block_arc.check_escaping_block_capture_legality_case(
                run_dir
            ),
        ),
        (
            "block-storage-arc-automation-semantics",
            lambda: domains.block_arc.check_block_storage_arc_automation_semantics_case(
                run_dir
            ),
        ),
        (
            "block-arc-runtime-abi",
            lambda: domains.block_arc.check_block_arc_runtime_abi_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "block-helper-runtime-execution",
            lambda: domains.block_arc.check_block_helper_runtime_execution_case(
                clangxx,
                run_dir,
            ),
        ),
        (
            "arc-property-helper",
            lambda: domains.block_arc.check_arc_property_helper_case(
                clangxx,
                run_dir,
            ),
        ),
    ]


def build_all_case_factories(context: CaseFactoryContext) -> LabeledCaseFactories:
    interop_packaging = build_interop_packaging_case_factories(context)
    concurrency = build_concurrency_case_factories(context)
    return [
        *build_core_case_factories(context),
        *build_registration_case_factories(context),
        *build_metaprogramming_case_factories(context),
        *interop_packaging[:2],
        *build_release_claim_case_factories(context),
        *interop_packaging[2:10],
        *concurrency[:5],
        *build_error_case_factories(context),
        *concurrency[5:],
        *build_cross_module_case_factories(context),
        *interop_packaging[10:],
        *build_object_model_case_factories(context),
        *build_storage_reflection_case_factories(context),
        *build_block_arc_case_factories(context),
    ]


__all__ = [
    "CaseFactory",
    "CaseFactoryContext",
    "LabeledCaseFactories",
    "build_all_case_factories",
    "build_block_arc_case_factories",
    "build_concurrency_case_factories",
    "build_core_case_factories",
    "build_cross_module_case_factories",
    "build_error_case_factories",
    "build_interop_packaging_case_factories",
    "build_metaprogramming_case_factories",
    "build_object_model_case_factories",
    "build_registration_case_factories",
    "build_release_claim_case_factories",
    "build_storage_reflection_case_factories",
]
