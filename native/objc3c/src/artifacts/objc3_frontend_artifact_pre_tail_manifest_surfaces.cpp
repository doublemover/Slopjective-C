#include "artifacts/objc3_frontend_artifact_pre_tail_manifest_surfaces.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_block_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_block_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_concurrency_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_concurrency_runtime_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_conformance_report_plan.h"
#include "artifacts/objc3_frontend_artifact_core_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_cross_module_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_dispatch_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_dispatch_runtime_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_error_handling_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_error_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_function_manifest.h"
#include "artifacts/objc3_frontend_artifact_interop_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_interop_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_metaprogramming_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_module_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_ownership_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_ownership_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_ownership_release_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_preservation_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_import_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_manifest_orchestration.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"
#include "artifacts/objc3_frontend_artifact_semantic_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_semantic_summary_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_source_closure_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_tooling_manifest_surfaces.h"
#include "artifacts/objc3_frontend_artifact_type_system_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_type_system_manifest_surfaces.h"
#include "artifacts/objc3_frontend_control_flow_semantic_artifacts.h"
#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

void WriteObjc3FrontendArtifactPreTailManifestSurfaces(
    std::ostream &manifest,
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendArtifactFunctionManifest &function_manifest,
    const Objc3FrontendArtifactCoreLoweringPlan &core_lowering_plan,
    const Objc3FrontendArtifactSemanticLoweringPlan &semantic_lowering_plan,
    const Objc3FrontendArtifactOwnershipAwareLoweringPlan
        &ownership_aware_lowering_plan,
    const Objc3FrontendArtifactBlockLoweringPlan &block_lowering_plan,
    const Objc3FrontendArtifactTypeSystemLoweringPlan
        &type_system_lowering_plan,
    const Objc3FrontendArtifactRuntimeImportPlan &runtime_import_plan,
    const Objc3FrontendArtifactModuleLoweringPlan &module_lowering_plan,
    const Objc3FrontendArtifactErrorLoweringPlan &error_lowering_plan,
    const Objc3FrontendArtifactInteropLoweringPlan &interop_lowering_plan,
    const Objc3FrontendArtifactPreservationPlan &artifact_preservation_plan,
    const Objc3FrontendArtifactConformanceReportPlan &conformance_report_plan,
    const Objc3FrontendArtifactRuntimeMetadataPlan &runtime_metadata_plan,
    const Objc3FrontendArtifactRuntimeRegistrationPlan
        &runtime_registration_plan,
    const Objc3TypeSystemTypeSemanticModelSummary
        &type_system_type_semantic_model_summary) {
  WriteObjc3FrontendRuntimeBootstrapManifestOrchestration(
      manifest, pipeline_result, conformance_report_plan, runtime_metadata_plan,
      runtime_registration_plan);
  manifest
      << ",\"objc_id_class_sel_object_pointer_typecheck_surface\":"
         "{\"id_typecheck_sites\":"
      << core_lowering_plan.id_class_sel_object_pointer_typecheck_contract
             .id_typecheck_sites
      << ",\"class_typecheck_sites\":"
      << core_lowering_plan.id_class_sel_object_pointer_typecheck_contract
             .class_typecheck_sites
      << ",\"sel_typecheck_sites\":"
      << core_lowering_plan.id_class_sel_object_pointer_typecheck_contract
             .sel_typecheck_sites
      << ",\"object_pointer_typecheck_sites\":"
      << core_lowering_plan.id_class_sel_object_pointer_typecheck_contract
             .object_pointer_typecheck_sites
      << ",\"total_typecheck_sites\":"
      << core_lowering_plan.id_class_sel_object_pointer_typecheck_contract
             .total_typecheck_sites
      << ",\"replay_key\":\""
      << core_lowering_plan.id_class_sel_object_pointer_typecheck_replay_key
      << "\",\"deterministic_handoff\":"
      << (core_lowering_plan.id_class_sel_object_pointer_typecheck_contract
                  .deterministic
              ? "true"
              : "false")
      << "}";
  WriteDispatchRuntimeAbiManifestSurfaces(
      manifest, core_lowering_plan.dispatch_surface_classification_snapshot,
      core_lowering_plan.dispatch_surface_classification_replay_key,
      core_lowering_plan.message_send_selector_lowering_snapshot,
      core_lowering_plan.message_send_selector_lowering_replay_key,
      core_lowering_plan.dispatch_abi_marshalling_snapshot,
      core_lowering_plan.dispatch_abi_marshalling_replay_key,
      core_lowering_plan.nil_receiver_semantics_foldability_snapshot,
      core_lowering_plan.nil_receiver_semantics_foldability_replay_key,
      core_lowering_plan.super_dispatch_method_family_snapshot,
      core_lowering_plan.super_dispatch_method_family_replay_key,
      core_lowering_plan.runtime_link_host_link_snapshot,
      core_lowering_plan.runtime_link_host_link_replay_key,
      core_lowering_plan.runtime_dispatch_lowering_abi_snapshot,
      core_lowering_plan.runtime_dispatch_lowering_abi_replay_key);
  WriteOwnershipReleaseManifestSurfaces(
      manifest,
      ownership_aware_lowering_plan.ownership_qualifier_lowering_contract,
      ownership_aware_lowering_plan.ownership_qualifier_lowering_replay_key,
      ownership_aware_lowering_plan.retain_release_operation_lowering_contract,
      ownership_aware_lowering_plan
          .retain_release_operation_lowering_replay_key,
      ownership_aware_lowering_plan.autoreleasepool_scope_lowering_contract,
      ownership_aware_lowering_plan.autoreleasepool_scope_lowering_replay_key,
      ownership_aware_lowering_plan.weak_unowned_semantics_lowering_contract,
      ownership_aware_lowering_plan.weak_unowned_semantics_lowering_replay_key,
      ownership_aware_lowering_plan.arc_diagnostics_fixit_lowering_contract,
      ownership_aware_lowering_plan.arc_diagnostics_fixit_lowering_replay_key);
  WriteBlockManifestSurfaces(
      manifest, block_lowering_plan.block_literal_capture_lowering_contract,
      block_lowering_plan.block_literal_capture_lowering_replay_key,
      block_lowering_plan.block_source_model_completion_contract,
      block_lowering_plan.block_source_model_completion_replay_key,
      block_lowering_plan.block_source_storage_annotation_contract,
      block_lowering_plan.block_source_storage_annotation_replay_key,
      block_lowering_plan.block_abi_invoke_trampoline_lowering_contract,
      block_lowering_plan.block_abi_invoke_trampoline_lowering_replay_key,
      block_lowering_plan.block_storage_escape_lowering_contract,
      block_lowering_plan.block_storage_escape_lowering_replay_key,
      block_lowering_plan.block_copy_dispose_lowering_contract,
      block_lowering_plan.block_copy_dispose_lowering_replay_key,
      block_lowering_plan.block_determinism_perf_baseline_lowering_contract,
      block_lowering_plan
          .block_determinism_perf_baseline_lowering_replay_key);
  WriteTypeSystemManifestSurfaces(
      manifest,
      type_system_lowering_plan.lightweight_generic_constraint_lowering_contract,
      type_system_lowering_plan
          .lightweight_generic_constraint_lowering_replay_key,
      type_system_lowering_plan
          .nullability_flow_warning_precision_lowering_contract,
      type_system_lowering_plan
          .nullability_flow_warning_precision_lowering_replay_key,
      type_system_lowering_plan
          .protocol_qualified_object_type_lowering_contract,
      type_system_lowering_plan
          .protocol_qualified_object_type_lowering_replay_key,
      type_system_lowering_plan.variance_bridge_cast_lowering_contract,
      type_system_lowering_plan.variance_bridge_cast_lowering_replay_key,
      type_system_lowering_plan.generic_metadata_abi_lowering_contract,
      type_system_lowering_plan.generic_metadata_abi_lowering_replay_key);
  manifest
      << ",\"objc_type_system_optional_keypath_lowering_contract\":"
      << BuildTypeSystemOptionalKeypathLoweringContractJson(
             core_lowering_plan.type_system_optional_keypath_lowering_contract,
             type_system_type_semantic_model_summary,
             type_system_type_semantic_model_summary.replay_key,
             core_lowering_plan.message_send_selector_lowering_replay_key,
             core_lowering_plan.dispatch_abi_marshalling_replay_key,
             core_lowering_plan.nil_receiver_semantics_foldability_replay_key,
             core_lowering_plan.type_system_optional_keypath_lowering_replay_key)
      << ",\"objc_control_flow_control_flow_safety_lowering_contract\":"
      << BuildControlFlowControlFlowSafetyLoweringContractJson(
             core_lowering_plan
                 .control_flow_control_flow_safety_lowering_contract,
             pipeline_result.control_flow_control_flow_semantic_model_summary,
             pipeline_result.control_flow_control_flow_semantic_model_summary
                 .replay_key,
             core_lowering_plan
                 .control_flow_control_flow_safety_lowering_replay_key)
      << ",\"objc_type_system_optional_keypath_runtime_helper_contract\":"
      << BuildTypeSystemOptionalKeypathRuntimeHelperContractJson(
             core_lowering_plan.type_system_optional_keypath_lowering_contract,
             runtime_registration_plan.runtime_support_library_link_wiring,
             core_lowering_plan.type_system_optional_keypath_lowering_replay_key);
  WriteCrossModuleManifestSurfaces(
      manifest, program, pipeline_result.parser_contract_snapshot,
      runtime_import_plan.module_import_graph_lowering_contract,
      runtime_import_plan.module_import_graph_lowering_replay_key,
      runtime_import_plan.runtime_aware_import_module_frontend_closure,
      runtime_import_plan
          .cross_module_runtime_metadata_semantic_preservation,
      runtime_import_plan.imported_runtime_metadata_semantic_rules,
      runtime_import_plan.serialized_runtime_metadata_import_lowering,
      runtime_import_plan.serialized_runtime_metadata_artifact_reuse,
      runtime_import_plan.cross_module_build_runtime_orchestration,
      module_lowering_plan.namespace_collision_shadowing_lowering_contract,
      module_lowering_plan.namespace_collision_shadowing_lowering_replay_key,
      module_lowering_plan.public_private_api_partition_lowering_contract,
      module_lowering_plan.public_private_api_partition_lowering_replay_key,
      module_lowering_plan
          .incremental_module_cache_invalidation_lowering_contract,
      module_lowering_plan
          .incremental_module_cache_invalidation_lowering_replay_key,
      module_lowering_plan.cross_module_conformance_lowering_contract,
      module_lowering_plan.cross_module_conformance_lowering_replay_key);
  WriteErrorHandlingManifestSurfaces(
      manifest, error_lowering_plan.throws_propagation_lowering_contract,
      error_lowering_plan.throws_propagation_lowering_replay_key,
      error_lowering_plan.result_like_lowering_contract,
      error_lowering_plan.result_like_lowering_replay_key,
      error_lowering_plan.ns_error_bridging_lowering_contract,
      error_lowering_plan.ns_error_bridging_lowering_replay_key,
      error_lowering_plan.unwind_cleanup_lowering_contract,
      error_lowering_plan.unwind_cleanup_lowering_replay_key,
      error_lowering_plan
          .deterministic_error_handling_throws_abi_propagation_lowering,
      interop_lowering_plan
          .error_handling_result_and_bridging_artifact_replay_summary);
  WriteSourceClosureManifestSurfaces(
      manifest, pipeline_result.object_pointer_nullability_generics_summary,
      pipeline_result.type_system_type_source_closure_summary,
      pipeline_result.control_flow_control_flow_source_closure_summary,
      pipeline_result.error_handling_error_source_closure_summary,
      pipeline_result.concurrency_async_source_closure_summary,
      pipeline_result.ownership_system_extension_source_closure_summary,
      pipeline_result
          .ownership_cleanup_resource_capture_source_completion_summary,
      pipeline_result
          .ownership_retainable_c_family_source_completion_summary,
      pipeline_result.dispatch_dispatch_intent_source_closure_summary,
      pipeline_result.dispatch_dispatch_intent_source_completion_summary,
      pipeline_result
          .metaprogramming_metaprogramming_source_closure_summary,
      pipeline_result
          .metaprogramming_macro_package_provenance_source_completion_summary,
      pipeline_result
          .metaprogramming_property_behavior_source_completion_summary,
      pipeline_result.interop_foreign_import_source_closure_summary,
      pipeline_result
          .interop_cpp_swift_interop_annotation_source_completion_summary);
  WriteToolingManifestSurfaces(
      manifest,
      pipeline_result.tooling_diagnostics_migrator_source_inventory_summary,
      pipeline_result.tooling_migration_canonicalization_source_completion_summary,
      pipeline_result.tooling_diagnostic_taxonomy_portability_contract_summary,
      pipeline_result.tooling_feature_specific_fixit_synthesis_summary,
      conformance_report_plan.tooling_legacy_canonical_migration_semantics_summary,
      conformance_report_plan
          .tooling_machine_readable_conformance_report_contract_summary,
      conformance_report_plan
          .tooling_feature_aware_conformance_report_emission_summary,
      conformance_report_plan
          .tooling_corpus_sharding_release_evidence_packaging_summary);
  WriteInteropManifestSurfaces(
      manifest, pipeline_result.interop_interop_semantic_model_summary,
      pipeline_result.interop_interop_runtime_parity_summary,
      pipeline_result.interop_cpp_interop_interaction_summary,
      pipeline_result.interop_swift_interop_isolation_summary,
      interop_lowering_plan.interop_foreign_surface_interface_preservation_summary,
      interop_lowering_plan.interop_header_module_bridge_generation_summary,
      interop_lowering_plan.interop_interop_lowering_contract,
      interop_lowering_plan.interop_interop_lowering_replay_key,
      interop_lowering_plan.interop_foreign_call_lifetime_lowering_contract,
      interop_lowering_plan.interop_foreign_call_lifetime_lowering_replay_key,
      interop_lowering_plan
          .interop_ffi_metadata_interface_preservation_contract,
      interop_lowering_plan.interop_ffi_metadata_interface_preservation_replay_key);
  WriteMetaprogrammingManifestSurfaces(
      manifest,
      pipeline_result
          .metaprogramming_expansion_behavior_semantic_model_summary,
      pipeline_result.metaprogramming_derive_expansion_inventory_summary,
      pipeline_result
          .metaprogramming_macro_safety_sandbox_determinism_summary,
      pipeline_result
          .metaprogramming_property_behavior_legality_compatibility_summary,
      pipeline_result
          .metaprogramming_property_behavior_source_completion_summary,
      semantic_lowering_plan.metaprogramming_expansion_lowering_contract,
      semantic_lowering_plan.metaprogramming_expansion_lowering_replay_key,
      semantic_lowering_plan
          .metaprogramming_synthesized_artifact_emission_contract,
      semantic_lowering_plan
          .metaprogramming_synthesized_artifact_emission_replay_key,
      artifact_preservation_plan
          .metaprogramming_module_interface_replay_preservation_summary);
  WriteDispatchManifestSurfaces(
      manifest, pipeline_result.dispatch_dispatch_intent_semantic_model_summary,
      pipeline_result.dispatch_dispatch_intent_legality_summary,
      pipeline_result.dispatch_dispatch_intent_compatibility_summary,
      semantic_lowering_plan.dispatch_dispatch_control_lowering_contract,
      semantic_lowering_plan.dispatch_dispatch_control_lowering_replay_key,
      artifact_preservation_plan
          .dispatch_dispatch_metadata_interface_preservation_summary);
  WriteConcurrencyManifestSurfaces(
      manifest,
      pipeline_result.concurrency_actor_member_isolation_source_closure_summary,
      pipeline_result.concurrency_actor_isolation_sendable_semantic_model_summary,
      pipeline_result
          .concurrency_actor_isolation_sendability_enforcement_summary,
      pipeline_result.concurrency_actor_race_hazard_escape_diagnostics_summary,
      semantic_lowering_plan.concurrency_actor_lowering_metadata_contract,
      semantic_lowering_plan.concurrency_actor_lowering_metadata_replay_key,
      pipeline_result.concurrency_task_group_cancellation_source_closure_summary,
      pipeline_result.concurrency_async_effect_suspension_semantic_model_summary,
      pipeline_result.concurrency_task_executor_cancellation_semantic_model_summary);
  WriteOwnershipManifestSurfaces(
      manifest,
      pipeline_result.ownership_system_extension_semantic_model_summary,
      pipeline_result.effects_ownership_semantic_model_summary,
      pipeline_result.cross_module_semantic_contracts_diagnostics_summary,
      pipeline_result.ownership_resource_move_use_after_move_semantics_summary,
      pipeline_result.ownership_borrowed_pointer_escape_analysis_summary,
      pipeline_result
          .ownership_capture_list_retainable_family_legality_completion_summary,
      semantic_lowering_plan.ownership_system_extension_lowering_contract,
      pipeline_result.ownership_system_extension_source_closure_summary,
      pipeline_result
          .ownership_retainable_c_family_source_completion_summary,
      semantic_lowering_plan.ownership_system_extension_lowering_replay_key,
      semantic_lowering_plan
          .ownership_borrowed_retainable_abi_completion_replay_key);
  WriteConcurrencyRuntimeManifestSurfaces(
      manifest,
      pipeline_result.concurrency_structured_task_cancellation_semantic_summary,
      pipeline_result.concurrency_executor_hop_affinity_compatibility_summary,
      pipeline_result.concurrency_await_suspension_resume_semantic_summary,
      pipeline_result.concurrency_async_diagnostics_compatibility_summary,
      semantic_lowering_plan.concurrency_async_continuation_lowering_contract,
      semantic_lowering_plan
          .concurrency_await_lowering_suspension_state_lowering_contract,
      semantic_lowering_plan.concurrency_async_continuation_lowering_replay_key,
      semantic_lowering_plan
          .concurrency_await_lowering_suspension_state_lowering_replay_key,
      pipeline_result.concurrency_task_executor_cancellation_semantic_model_summary,
      semantic_lowering_plan
          .concurrency_actor_isolation_sendability_lowering_contract,
      semantic_lowering_plan
          .concurrency_actor_isolation_sendability_lowering_replay_key,
      semantic_lowering_plan
          .concurrency_task_runtime_interop_cancellation_lowering_contract,
      semantic_lowering_plan
          .concurrency_task_runtime_interop_cancellation_lowering_replay_key,
      semantic_lowering_plan
          .concurrency_concurrency_replay_race_guard_lowering_contract,
      semantic_lowering_plan
          .concurrency_concurrency_replay_race_guard_lowering_replay_key,
      pipeline_result.concurrency_async_source_closure_summary,
      core_lowering_plan.control_flow_control_flow_safety_lowering_contract,
      core_lowering_plan.control_flow_control_flow_safety_lowering_replay_key,
      ownership_aware_lowering_plan.autoreleasepool_scope_lowering_contract,
      ownership_aware_lowering_plan.autoreleasepool_scope_lowering_replay_key);
  WriteSemanticSummaryManifestSurfaces(
      manifest, pipeline_result.error_handling_error_semantic_model_summary,
      pipeline_result.error_handling_try_do_catch_semantic_summary,
      pipeline_result.error_handling_error_bridge_legality_summary,
      pipeline_result.control_flow_control_flow_semantic_model_summary,
      type_system_type_semantic_model_summary,
      pipeline_result.symbol_graph_scope_resolution_summary,
      function_manifest);
}

}  // namespace objc3::artifacts::frontend
