#include "artifacts/objc3_frontend_artifact_ir_application.h"

#include "artifacts/objc3_frontend_artifact_arc_ownership_metadata.h"
#include "artifacts/objc3_frontend_artifact_block_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_block_metadata.h"
#include "artifacts/objc3_frontend_artifact_concurrency_metadata.h"
#include "artifacts/objc3_frontend_artifact_concurrency_runtime_metadata.h"
#include "artifacts/objc3_frontend_artifact_conformance_report_plan.h"
#include "artifacts/objc3_frontend_artifact_core_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_dispatch_metadata_application.h"
#include "artifacts/objc3_frontend_artifact_error_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_error_metadata.h"
#include "artifacts/objc3_frontend_artifact_interop_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_interop_metadata.h"
#include "artifacts/objc3_frontend_artifact_ir_emission_completion.h"
#include "artifacts/objc3_frontend_artifact_metaprogramming_metadata.h"
#include "artifacts/objc3_frontend_artifact_module_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_module_metadata.h"
#include "artifacts/objc3_frontend_artifact_ownership_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_ownership_metadata.h"
#include "artifacts/objc3_frontend_artifact_preservation_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_import_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_contract_metadata.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"
#include "artifacts/objc3_frontend_artifact_semantic_closure_metadata.h"
#include "artifacts/objc3_frontend_artifact_semantic_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_source_linkage_metadata.h"
#include "artifacts/objc3_frontend_artifact_type_system_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_type_system_metadata.h"
#include "artifacts/objc3_frontend_artifacts.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "support/frontend_dispatch_contract_records.h"
#include "support/objc3_frontend_dispatch_metadata_ir_application.h"

namespace objc3::artifacts::frontend {

void FinalizeObjc3FrontendArtifactIrApplication(
    const Objc3FrontendArtifactIrApplicationInputs &inputs) {
  if (FinalizeObjc3FrontendPostPipelineFailure(
          inputs.bundle, inputs.options, inputs.post_pipeline_failure)) {
    return;
  }

  if (!inputs.options.emit_ir && !inputs.options.emit_object) {
    return;
  }

  const auto &pipeline_result = inputs.pipeline_result;
  const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff =
      pipeline_result.sema_type_metadata_handoff;
  const auto dispatch_metadata_preservation_snapshot =
      BuildDispatchMetadataPreservationSnapshot(
          inputs.artifact_preservation_plan
              .dispatch_dispatch_metadata_interface_preservation_summary);

  Objc3IRFrontendMetadata ir_frontend_metadata;
  ApplyObjc3FrontendSourceLinkageMetadata(
      ir_frontend_metadata, inputs.options,
      inputs.conformance_report_plan.versioned_conformance_report_lowering,
      pipeline_result.canonical_literal_rejection_counts,
      type_metadata_handoff.interface_implementation_summary,
      pipeline_result.protocol_category_summary,
      pipeline_result.class_protocol_category_linking_summary,
      pipeline_result.selector_normalization_summary,
      pipeline_result.property_attribute_summary);
  ApplyObjc3ObjectDispatchMetadataIrApplication(
      ir_frontend_metadata,
      BuildObjc3ObjectDispatchMetadataApplication(
          inputs.core_lowering_plan.property_synthesis_ivar_binding_replay_key,
          inputs.core_lowering_plan.property_synthesis_ivar_binding_snapshot,
          inputs.core_lowering_plan
              .id_class_sel_object_pointer_typecheck_replay_key,
          inputs.core_lowering_plan
              .id_class_sel_object_pointer_typecheck_snapshot,
          inputs.core_lowering_plan.dispatch_surface_classification_replay_key,
          inputs.core_lowering_plan.dispatch_surface_classification_snapshot,
          inputs.core_lowering_plan.message_send_selector_lowering_replay_key,
          inputs.core_lowering_plan.message_send_selector_lowering_snapshot,
          inputs.core_lowering_plan.dispatch_abi_marshalling_replay_key,
          inputs.core_lowering_plan.dispatch_abi_marshalling_snapshot,
          inputs.core_lowering_plan
              .nil_receiver_semantics_foldability_replay_key,
          inputs.core_lowering_plan.nil_receiver_semantics_foldability_snapshot,
          inputs.core_lowering_plan.super_dispatch_method_family_replay_key,
          inputs.core_lowering_plan.super_dispatch_method_family_snapshot,
          inputs.core_lowering_plan.runtime_link_host_link_replay_key,
          inputs.core_lowering_plan.runtime_link_host_link_snapshot));
  ApplyObjc3FrontendConcurrencyMetadata(
      ir_frontend_metadata,
      inputs.semantic_lowering_plan
          .concurrency_async_continuation_lowering_replay_key,
      inputs.semantic_lowering_plan
          .concurrency_async_continuation_lowering_contract,
      inputs.semantic_lowering_plan
          .concurrency_await_lowering_suspension_state_lowering_replay_key,
      inputs.semantic_lowering_plan
          .concurrency_await_lowering_suspension_state_lowering_contract,
      inputs.semantic_lowering_plan
          .concurrency_actor_isolation_sendability_lowering_replay_key,
      inputs.semantic_lowering_plan
          .concurrency_actor_isolation_sendability_lowering_contract,
      inputs.semantic_lowering_plan.concurrency_actor_lowering_metadata_replay_key,
      inputs.semantic_lowering_plan.concurrency_actor_lowering_metadata_contract);
  ApplyObjc3FrontendMetaprogrammingMetadata(
      ir_frontend_metadata,
      inputs.semantic_lowering_plan.metaprogramming_expansion_lowering_replay_key,
      inputs.semantic_lowering_plan.metaprogramming_expansion_lowering_contract,
      inputs.semantic_lowering_plan
          .metaprogramming_synthesized_artifact_emission_replay_key,
      inputs.semantic_lowering_plan
          .metaprogramming_synthesized_artifact_emission_contract,
      inputs.semantic_lowering_plan.metaprogramming_derived_method_bundles,
      inputs.semantic_lowering_plan.metaprogramming_macro_artifact_bundles,
      inputs.semantic_lowering_plan
          .metaprogramming_property_behavior_artifact_bundles,
      inputs.artifact_preservation_plan
          .metaprogramming_module_interface_replay_preservation_summary);
  ApplyObjc3FrontendInteropMetadata(
      ir_frontend_metadata,
      inputs.interop_lowering_plan.interop_interop_lowering_replay_key,
      inputs.interop_lowering_plan.interop_interop_lowering_contract,
      inputs.interop_lowering_plan
          .interop_foreign_call_lifetime_lowering_replay_key,
      inputs.interop_lowering_plan.interop_foreign_call_lifetime_lowering_contract,
      inputs.interop_lowering_plan
          .interop_ffi_metadata_interface_preservation_replay_key,
      inputs.interop_lowering_plan
          .interop_ffi_metadata_interface_preservation_contract,
      inputs.interop_lowering_plan.interop_header_module_bridge_generation_summary);
  ApplyObjc3DispatchMetadataIrApplication(
      ir_frontend_metadata,
      BuildObjc3DispatchMetadataApplication(
          inputs.semantic_lowering_plan
              .dispatch_dispatch_control_lowering_replay_key,
          inputs.semantic_lowering_plan.dispatch_dispatch_control_lowering_snapshot,
          dispatch_metadata_preservation_snapshot));
  ApplyObjc3FrontendOwnershipMetadata(
      ir_frontend_metadata,
      inputs.semantic_lowering_plan.ownership_system_extension_lowering_replay_key,
      inputs.semantic_lowering_plan.ownership_system_extension_lowering_contract,
      inputs.semantic_lowering_plan
          .ownership_borrowed_retainable_abi_completion_replay_key,
      pipeline_result.ownership_system_extension_source_closure_summary,
      pipeline_result.ownership_retainable_c_family_source_completion_summary);
  ApplyObjc3FrontendConcurrencyRuntimeMetadata(
      ir_frontend_metadata,
      inputs.semantic_lowering_plan
          .concurrency_task_runtime_interop_cancellation_lowering_replay_key,
      inputs.semantic_lowering_plan
          .concurrency_task_runtime_interop_cancellation_lowering_contract,
      inputs.semantic_lowering_plan
          .concurrency_concurrency_replay_race_guard_lowering_replay_key,
      inputs.semantic_lowering_plan
          .concurrency_concurrency_replay_race_guard_lowering_contract);
  ApplyObjc3FrontendArcOwnershipMetadata(
      ir_frontend_metadata,
      inputs.ownership_aware_lowering_plan.ownership_qualifier_lowering_replay_key,
      inputs.ownership_aware_lowering_plan.ownership_qualifier_lowering_contract,
      inputs.ownership_aware_lowering_plan
          .retain_release_operation_lowering_replay_key,
      inputs.ownership_aware_lowering_plan.retain_release_operation_lowering_contract,
      inputs.ownership_aware_lowering_plan
          .autoreleasepool_scope_lowering_replay_key,
      inputs.ownership_aware_lowering_plan.autoreleasepool_scope_lowering_contract,
      inputs.ownership_aware_lowering_plan.weak_unowned_semantics_lowering_replay_key,
      inputs.ownership_aware_lowering_plan.weak_unowned_semantics_lowering_contract,
      inputs.ownership_aware_lowering_plan.arc_diagnostics_fixit_lowering_replay_key,
      inputs.ownership_aware_lowering_plan.arc_diagnostics_fixit_lowering_contract);
  ApplyObjc3FrontendBlockMetadata(
      ir_frontend_metadata,
      inputs.block_lowering_plan.block_literal_capture_lowering_replay_key,
      inputs.block_lowering_plan.block_literal_capture_lowering_contract,
      inputs.block_lowering_plan.block_source_model_completion_replay_key,
      inputs.block_lowering_plan.block_source_model_completion_contract,
      inputs.block_lowering_plan.block_source_storage_annotation_replay_key,
      inputs.block_lowering_plan.block_source_storage_annotation_contract,
      inputs.block_lowering_plan.block_abi_invoke_trampoline_lowering_replay_key,
      inputs.block_lowering_plan.block_abi_invoke_trampoline_lowering_contract,
      inputs.block_lowering_plan.block_storage_escape_lowering_replay_key,
      inputs.block_lowering_plan.block_storage_escape_lowering_contract,
      inputs.block_lowering_plan.block_copy_dispose_lowering_replay_key,
      inputs.block_lowering_plan.block_copy_dispose_lowering_contract,
      inputs.block_lowering_plan
          .block_determinism_perf_baseline_lowering_replay_key,
      inputs.block_lowering_plan
          .block_determinism_perf_baseline_lowering_contract);
  ApplyObjc3FrontendTypeSystemMetadata(
      ir_frontend_metadata,
      inputs.type_system_lowering_plan
          .lightweight_generic_constraint_lowering_replay_key,
      inputs.type_system_lowering_plan
          .lightweight_generic_constraint_lowering_contract,
      inputs.type_system_lowering_plan
          .nullability_flow_warning_precision_lowering_replay_key,
      inputs.type_system_lowering_plan
          .nullability_flow_warning_precision_lowering_contract,
      inputs.type_system_lowering_plan
          .protocol_qualified_object_type_lowering_replay_key,
      inputs.type_system_lowering_plan
          .protocol_qualified_object_type_lowering_contract,
      inputs.type_system_lowering_plan.variance_bridge_cast_lowering_replay_key,
      inputs.type_system_lowering_plan.variance_bridge_cast_lowering_contract,
      inputs.type_system_lowering_plan.generic_metadata_abi_lowering_replay_key,
      inputs.type_system_lowering_plan.generic_metadata_abi_lowering_contract);
  ApplyObjc3FrontendModuleMetadata(
      ir_frontend_metadata,
      inputs.runtime_import_plan.module_import_graph_lowering_replay_key,
      inputs.runtime_import_plan.module_import_graph_lowering_contract,
      inputs.module_lowering_plan.namespace_collision_shadowing_lowering_replay_key,
      inputs.module_lowering_plan.namespace_collision_shadowing_lowering_contract,
      inputs.module_lowering_plan.public_private_api_partition_lowering_replay_key,
      inputs.module_lowering_plan.public_private_api_partition_lowering_contract,
      inputs.module_lowering_plan
          .incremental_module_cache_invalidation_lowering_replay_key,
      inputs.module_lowering_plan
          .incremental_module_cache_invalidation_lowering_contract,
      inputs.module_lowering_plan.cross_module_conformance_lowering_replay_key,
      inputs.module_lowering_plan.cross_module_conformance_lowering_contract);
  ApplyObjc3FrontendErrorMetadata(
      ir_frontend_metadata,
      inputs.error_lowering_plan.throws_propagation_lowering_replay_key,
      inputs.error_lowering_plan.throws_propagation_lowering_contract,
      inputs.error_lowering_plan
          .error_handling_throws_abi_propagation_lowering_replay_key,
      inputs.error_lowering_plan.result_like_lowering_replay_key,
      inputs.error_lowering_plan.result_like_lowering_contract,
      inputs.error_lowering_plan.ns_error_bridging_lowering_replay_key,
      inputs.error_lowering_plan.ns_error_bridging_lowering_contract,
      inputs.error_lowering_plan.unwind_cleanup_lowering_replay_key,
      inputs.error_lowering_plan.unwind_cleanup_lowering_contract,
      inputs.interop_lowering_plan
          .error_handling_result_and_bridging_artifact_replay_summary);
  ApplyObjc3FrontendSemanticClosureMetadata(
      ir_frontend_metadata,
      pipeline_result.object_pointer_nullability_generics_summary,
      pipeline_result.symbol_graph_scope_resolution_summary,
      pipeline_result.sema_parity_surface
          .deterministic_interface_implementation_handoff,
      type_metadata_handoff.interface_implementation_summary,
      pipeline_result.protocol_category_summary,
      pipeline_result.class_protocol_category_linking_summary,
      pipeline_result.selector_normalization_summary,
      pipeline_result.property_attribute_summary);
  ApplyObjc3FrontendRuntimeMetadataContractMetadata(
      ir_frontend_metadata, pipeline_result.runtime_metadata_source_ownership_boundary,
      pipeline_result.runtime_export_legality_boundary,
      pipeline_result.runtime_export_enforcement_summary,
      inputs.runtime_metadata_plan.runtime_metadata_section_abi,
      inputs.runtime_metadata_plan.runtime_metadata_section_publication,
      pipeline_result.executable_metadata_typed_lowering_handoff, inputs.input_path,
      inputs.bundle.parse_lowering_readiness_surface.parse_artifact_replay_key,
      inputs.bundle.parse_lowering_readiness_surface.lowering_boundary_replay_key);
  ApplyObjc3FrontendFinalRuntimeAndReadinessMetadata(
      ir_frontend_metadata, inputs.bundle,
      pipeline_result.executable_metadata_typed_lowering_handoff,
      inputs.runtime_metadata_plan.runtime_metadata_section_publication,
      inputs.runtime_metadata_plan.runtime_metadata_object_inspection,
      inputs.runtime_metadata_plan.executable_metadata_debug_projection,
      inputs.runtime_registration_plan.runtime_support_library,
      inputs.runtime_registration_plan.runtime_support_library_core_feature,
      inputs.runtime_registration_plan.runtime_support_library_link_wiring,
      inputs.ownership_aware_lowering_plan.ownership_aware_lowering_behavior_scaffold,
      pipeline_result.ir_emission_completeness_scaffold,
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
      inputs.ir_emission_core_feature_impl_surface);
  CompleteObjc3FrontendArtifactIREmission(
      inputs.bundle, pipeline_result, inputs.options, inputs.program,
      ir_frontend_metadata,
      Objc3RuntimeDispatchLoweringAbiBoundarySummary(
          inputs.core_lowering_plan.runtime_dispatch_lowering_abi_contract),
      inputs.core_lowering_plan.message_send_selector_lowering_contract
          .message_send_sites);
}

}  // namespace objc3::artifacts::frontend
