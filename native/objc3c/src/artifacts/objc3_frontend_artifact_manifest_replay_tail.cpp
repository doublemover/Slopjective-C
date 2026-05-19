#include "artifacts/objc3_frontend_artifact_manifest_replay_tail.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_block_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_core_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_error_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_function_manifest.h"
#include "artifacts/objc3_frontend_artifact_interop_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_lowering_replay_manifest.h"
#include "artifacts/objc3_frontend_artifact_manifest_pipeline.h"
#include "artifacts/objc3_frontend_artifact_module_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_ownership_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_import_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_plan.h"
#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"
#include "artifacts/objc3_frontend_artifact_source_shape_plan.h"
#include "artifacts/objc3_frontend_artifact_type_system_lowering_plan.h"
#include "artifacts/objc3_frontend_artifacts_runtime_manifest_surfaces.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactManifestReplayTail(
    std::ostream &manifest,
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactFunctionManifest &function_manifest,
    const Objc3FrontendArtifactSourceShapePlan &source_shape_plan,
    const Objc3FrontendArtifactCoreLoweringPlan &core_lowering_plan,
    const Objc3FrontendArtifactOwnershipAwareLoweringPlan
        &ownership_aware_lowering_plan,
    const Objc3FrontendArtifactBlockLoweringPlan &block_lowering_plan,
    const Objc3FrontendArtifactTypeSystemLoweringPlan
        &type_system_lowering_plan,
    const Objc3FrontendArtifactRuntimeImportPlan &runtime_import_plan,
    const Objc3FrontendArtifactModuleLoweringPlan &module_lowering_plan,
    const Objc3FrontendArtifactErrorLoweringPlan &error_lowering_plan,
    const Objc3FrontendArtifactInteropLoweringPlan &interop_lowering_plan,
    const Objc3FrontendArtifactRuntimeMetadataPlan &runtime_metadata_plan,
    const Objc3FrontendArtifactRuntimeRegistrationPlan
        &runtime_registration_plan) {
  manifest << "    }\n";
  manifest << "  },\n";
  const Objc3FrontendArtifactManifestLoweringHeaderFields
      manifest_lowering_header_fields{
          function_manifest.vector_signature_functions,
          core_lowering_plan.property_synthesis_ivar_binding_replay_key,
          core_lowering_plan.property_synthesis_ivar_binding_contract
              .deterministic};
  AppendObjc3FrontendArtifactManifestLoweringHeader(
      manifest, options, manifest_lowering_header_fields);
  const Objc3RuntimeDispatchTableReflectionRecordLoweringFields
      runtime_dispatch_table_reflection_record_lowering_fields{
          core_lowering_plan.message_send_selector_lowering_contract
              .message_send_sites};
  const bool dispatch_accessor_deterministic_handoff =
      core_lowering_plan.property_synthesis_ivar_binding_contract
          .deterministic &&
      core_lowering_plan.dispatch_surface_classification_contract
          .deterministic &&
      core_lowering_plan.message_send_selector_lowering_contract.deterministic &&
      core_lowering_plan.runtime_link_host_link_contract.deterministic;
  Objc3DispatchAndSynthesizedAccessorLoweringFields
      dispatch_and_synthesized_accessor_lowering_fields;
  dispatch_and_synthesized_accessor_lowering_fields.runtime_dispatch_symbol =
      core_lowering_plan.runtime_link_host_link_contract.runtime_dispatch_symbol;
  dispatch_and_synthesized_accessor_lowering_fields.runtime_dispatch_arg_slots =
      core_lowering_plan.runtime_link_host_link_contract
          .runtime_dispatch_arg_slots;
  dispatch_and_synthesized_accessor_lowering_fields
      .runtime_dispatch_declaration_parameter_count =
      core_lowering_plan.runtime_link_host_link_contract
          .runtime_dispatch_declaration_parameter_count;
  dispatch_and_synthesized_accessor_lowering_fields
      .runtime_dispatch_symbol_matches_lowering =
      core_lowering_plan.runtime_link_host_link_contract
          .runtime_dispatch_symbol == options.lowering.runtime_dispatch_symbol &&
      core_lowering_plan.runtime_link_host_link_contract
          .runtime_dispatch_symbol ==
          runtime_registration_plan.runtime_support_library_link_wiring
              .runtime_dispatch_symbol;
  dispatch_and_synthesized_accessor_lowering_fields
      .live_runtime_dispatch_sites =
      core_lowering_plan.dispatch_surface_classification_contract
          .instance_dispatch_sites +
      core_lowering_plan.dispatch_surface_classification_contract
          .class_dispatch_sites +
      core_lowering_plan.dispatch_surface_classification_contract
          .super_dispatch_sites +
      core_lowering_plan.dispatch_surface_classification_contract
          .dynamic_dispatch_sites;
  dispatch_and_synthesized_accessor_lowering_fields.direct_dispatch_sites =
      core_lowering_plan.dispatch_surface_classification_contract
          .direct_dispatch_sites;
  dispatch_and_synthesized_accessor_lowering_fields.message_send_sites =
      core_lowering_plan.message_send_selector_lowering_contract
          .message_send_sites;
  dispatch_and_synthesized_accessor_lowering_fields.property_synthesis_sites =
      core_lowering_plan.property_synthesis_ivar_binding_contract
          .property_synthesis_sites;
  dispatch_and_synthesized_accessor_lowering_fields
      .property_synthesis_explicit_ivar_bindings =
      core_lowering_plan.property_synthesis_ivar_binding_contract
          .property_synthesis_explicit_ivar_bindings;
  dispatch_and_synthesized_accessor_lowering_fields
      .property_synthesis_default_ivar_bindings =
      core_lowering_plan.property_synthesis_ivar_binding_contract
          .property_synthesis_default_ivar_bindings;
  dispatch_and_synthesized_accessor_lowering_fields
      .interface_owned_property_synthesis_sites =
      core_lowering_plan.property_synthesis_ivar_binding_contract
          .interface_owned_property_synthesis_sites;
  dispatch_and_synthesized_accessor_lowering_fields
      .implementation_property_redeclaration_sites =
      core_lowering_plan.property_synthesis_ivar_binding_contract
          .implementation_property_redeclaration_sites;
  dispatch_and_synthesized_accessor_lowering_fields.ivar_binding_resolved =
      core_lowering_plan.property_synthesis_ivar_binding_contract
          .ivar_binding_resolved;
  dispatch_and_synthesized_accessor_lowering_fields.deterministic_handoff =
      dispatch_accessor_deterministic_handoff;
  Objc3DispatchAccessorRuntimeAbiFields dispatch_accessor_runtime_abi_fields;
  dispatch_accessor_runtime_abi_fields.runtime_dispatch_symbol =
      core_lowering_plan.runtime_link_host_link_contract.runtime_dispatch_symbol;
  dispatch_accessor_runtime_abi_fields.deterministic =
      dispatch_accessor_deterministic_handoff;
  const Objc3StorageAccessorRuntimeAbiFields storage_accessor_runtime_abi_fields{
      core_lowering_plan.property_synthesis_ivar_binding_contract
          .deterministic &&
      core_lowering_plan.runtime_link_host_link_contract.deterministic};
  WriteObjc3FrontendRuntimeManifestSurfaces(
      manifest,
      runtime_registration_plan.runtime_translation_unit_registration_manifest,
      pipeline_result.runtime_metadata_source_records,
      pipeline_result.executable_metadata_source_graph,
      dispatch_and_synthesized_accessor_lowering_fields,
      dispatch_accessor_runtime_abi_fields,
      runtime_dispatch_table_reflection_record_lowering_fields,
      storage_accessor_runtime_abi_fields,
      runtime_metadata_plan.runtime_metadata_section_publication,
      runtime_registration_plan.runtime_bootstrap_api,
      runtime_registration_plan.runtime_bootstrap_semantics,
      runtime_registration_plan.runtime_registration_descriptor_frontend_closure,
      runtime_registration_plan
          .runtime_registration_descriptor_image_root_source_surface,
      runtime_registration_plan.runtime_bootstrap_lowering,
      runtime_registration_plan.runtime_bootstrap_legality_semantics,
      runtime_registration_plan.runtime_bootstrap_failure_restart_semantics);
  WriteObjc3FrontendDispatchReplayManifestEntries(
      manifest,
      core_lowering_plan.id_class_sel_object_pointer_typecheck_replay_key,
      core_lowering_plan.id_class_sel_object_pointer_typecheck_contract,
      core_lowering_plan.dispatch_surface_classification_replay_key,
      core_lowering_plan.dispatch_surface_classification_contract,
      core_lowering_plan.message_send_selector_lowering_replay_key,
      core_lowering_plan.message_send_selector_lowering_contract,
      core_lowering_plan.dispatch_abi_marshalling_replay_key,
      core_lowering_plan.dispatch_abi_marshalling_contract,
      core_lowering_plan.nil_receiver_semantics_foldability_replay_key,
      core_lowering_plan.nil_receiver_semantics_foldability_contract,
      core_lowering_plan.control_flow_control_flow_safety_lowering_replay_key,
      core_lowering_plan.control_flow_control_flow_safety_lowering_contract,
      core_lowering_plan.super_dispatch_method_family_replay_key,
      core_lowering_plan.super_dispatch_method_family_contract,
      core_lowering_plan.runtime_link_host_link_replay_key,
      core_lowering_plan.runtime_link_host_link_contract,
      runtime_registration_plan.runtime_support_library_link_wiring);
  WriteObjc3FrontendOwnershipAndBlockReplayManifestEntries(
      manifest,
      ownership_aware_lowering_plan.ownership_qualifier_lowering_replay_key,
      ownership_aware_lowering_plan.ownership_qualifier_lowering_contract,
      ownership_aware_lowering_plan.retain_release_operation_lowering_replay_key,
      ownership_aware_lowering_plan.retain_release_operation_lowering_contract,
      ownership_aware_lowering_plan.autoreleasepool_scope_lowering_replay_key,
      ownership_aware_lowering_plan.autoreleasepool_scope_lowering_contract,
      ownership_aware_lowering_plan.weak_unowned_semantics_lowering_replay_key,
      ownership_aware_lowering_plan.weak_unowned_semantics_lowering_contract,
      ownership_aware_lowering_plan.arc_diagnostics_fixit_lowering_replay_key,
      ownership_aware_lowering_plan.arc_diagnostics_fixit_lowering_contract,
      block_lowering_plan.block_literal_capture_lowering_replay_key,
      block_lowering_plan.block_literal_capture_lowering_contract,
      block_lowering_plan.block_abi_invoke_trampoline_lowering_replay_key,
      block_lowering_plan.block_abi_invoke_trampoline_lowering_contract,
      block_lowering_plan.block_storage_escape_lowering_replay_key,
      block_lowering_plan.block_storage_escape_lowering_contract,
      block_lowering_plan.block_copy_dispose_lowering_replay_key,
      block_lowering_plan.block_copy_dispose_lowering_contract,
      block_lowering_plan
          .block_determinism_perf_baseline_lowering_replay_key,
      block_lowering_plan
          .block_determinism_perf_baseline_lowering_contract);
  WriteObjc3FrontendTypeAndModuleReplayManifestEntries(
      manifest,
      type_system_lowering_plan
          .lightweight_generic_constraint_lowering_replay_key,
      type_system_lowering_plan.lightweight_generic_constraint_lowering_contract,
      type_system_lowering_plan
          .nullability_flow_warning_precision_lowering_replay_key,
      type_system_lowering_plan
          .nullability_flow_warning_precision_lowering_contract,
      type_system_lowering_plan
          .protocol_qualified_object_type_lowering_replay_key,
      type_system_lowering_plan
          .protocol_qualified_object_type_lowering_contract,
      type_system_lowering_plan.variance_bridge_cast_lowering_replay_key,
      type_system_lowering_plan.variance_bridge_cast_lowering_contract,
      type_system_lowering_plan.generic_metadata_abi_lowering_replay_key,
      type_system_lowering_plan.generic_metadata_abi_lowering_contract,
      runtime_import_plan.module_import_graph_lowering_replay_key,
      runtime_import_plan.module_import_graph_lowering_contract,
      module_lowering_plan.namespace_collision_shadowing_lowering_replay_key,
      module_lowering_plan.namespace_collision_shadowing_lowering_contract,
      module_lowering_plan.public_private_api_partition_lowering_replay_key,
      module_lowering_plan.public_private_api_partition_lowering_contract,
      module_lowering_plan
          .incremental_module_cache_invalidation_lowering_replay_key,
      module_lowering_plan
          .incremental_module_cache_invalidation_lowering_contract,
      module_lowering_plan.cross_module_conformance_lowering_replay_key,
      module_lowering_plan.cross_module_conformance_lowering_contract);
  WriteObjc3FrontendErrorReplayManifestEntries(
      manifest, error_lowering_plan.throws_propagation_lowering_replay_key,
      error_lowering_plan.throws_propagation_lowering_contract,
      error_lowering_plan.result_like_lowering_replay_key,
      error_lowering_plan.result_like_lowering_contract,
      error_lowering_plan.ns_error_bridging_lowering_replay_key,
      error_lowering_plan.ns_error_bridging_lowering_contract,
      error_lowering_plan.unwind_cleanup_lowering_replay_key,
      error_lowering_plan.unwind_cleanup_lowering_contract,
      error_lowering_plan
          .error_handling_throws_abi_propagation_lowering_replay_key,
      error_lowering_plan
          .deterministic_error_handling_throws_abi_propagation_lowering,
      interop_lowering_plan
          .error_handling_result_and_bridging_artifact_replay_summary);
  AppendObjc3FrontendArtifactManifestRecordArrays(
      manifest, program, source_shape_plan.resolved_global_values,
      function_manifest.manifest_functions,
      pipeline_result.sema_type_metadata_handoff,
      pipeline_result.runtime_metadata_source_records);
  manifest << "}\n";
}

}  // namespace objc3::artifacts::frontend
