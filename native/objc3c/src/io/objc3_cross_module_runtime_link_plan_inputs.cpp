#include "io/objc3_cross_module_runtime_link_plan_inputs.h"

bool TryValidateObjc3CrossModuleRuntimeLinkPlanArtifactInputs(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    std::string &error) {
  if (inputs.contract_id.empty() ||
      inputs.source_orchestration_contract_id.empty() ||
      inputs.import_surface_contract_id.empty() ||
      inputs.registration_manifest_contract_id.empty() ||
      inputs.payload_model.empty() ||
      inputs.artifact_relative_path.empty() ||
      inputs.linker_response_artifact_relative_path.empty() ||
      inputs.authority_model.empty() ||
      inputs.packaging_model.empty() ||
      inputs.registration_scope_model.empty() ||
      inputs.link_object_order_model.empty() ||
      inputs.local_module_name.empty() ||
      inputs.local_import_surface_artifact_relative_path.empty() ||
      inputs.local_registration_manifest_artifact_relative_path.empty() ||
      inputs.local_object_artifact_relative_path.empty() ||
      inputs.runtime_support_library_archive_relative_path.empty() ||
      inputs.object_format.empty() ||
      inputs.local_translation_unit_identity_model.empty() ||
      inputs.local_translation_unit_identity_key.empty() ||
      inputs.local_translation_unit_registration_order_ordinal == 0 ||
      inputs.local_total_descriptor_count !=
          inputs.local_class_descriptor_count +
              inputs.local_protocol_descriptor_count +
              inputs.local_category_descriptor_count +
              inputs.local_property_descriptor_count +
              inputs.local_ivar_descriptor_count ||
      inputs.expected_error_handling_contract_id.empty() ||
      inputs.expected_error_handling_source_contract_id.empty() ||
      inputs.expected_concurrency_actor_contract_id.empty() ||
      inputs.expected_concurrency_actor_source_contract_id.empty() ||
      inputs.expected_interop_ffi_contract_id.empty() ||
      inputs.expected_interop_ffi_source_contract_id.empty() ||
      inputs.expected_interop_ffi_preservation_contract_id.empty() ||
      inputs.expected_interop_header_module_bridge_contract_id.empty() ||
      inputs.expected_interop_header_module_bridge_source_contract_id.empty() ||
      inputs.expected_interop_header_module_bridge_preservation_contract_id
          .empty() ||
      inputs.expected_interop_bridge_header_artifact_relative_path.empty() ||
      inputs.expected_interop_bridge_module_artifact_relative_path.empty() ||
      inputs.expected_interop_bridge_artifact_relative_path.empty() ||
      inputs.expected_metaprogramming_host_cache_contract_id.empty() ||
      inputs.expected_metaprogramming_host_cache_source_contract_id.empty() ||
      inputs.expected_metaprogramming_host_cache_executable_relative_path
          .empty() ||
      inputs.expected_metaprogramming_host_cache_root_relative_path.empty() ||
      inputs.expected_block_ownership_contract_id.empty() ||
      inputs.expected_block_ownership_source_contract_id.empty() ||
      inputs
          .expected_block_ownership_object_invoke_thunk_lowering_contract_id
          .empty() ||
      inputs.expected_block_ownership_byref_helper_lowering_contract_id
          .empty() ||
      inputs.expected_block_ownership_escape_runtime_hook_lowering_contract_id
          .empty() ||
      inputs
          .expected_block_ownership_runtime_support_library_link_wiring_contract_id
          .empty() ||
      inputs
          .expected_block_ownership_retain_release_operation_lowering_contract_id
          .empty() ||
      inputs
          .expected_block_ownership_autoreleasepool_scope_lowering_contract_id
          .empty() ||
      inputs.expected_storage_reflection_contract_id.empty() ||
      inputs.expected_storage_reflection_source_contract_id.empty() ||
      inputs
          .expected_storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id
          .empty() ||
      inputs
          .expected_storage_reflection_executable_property_accessor_layout_lowering_contract_id
          .empty() ||
      inputs
          .expected_storage_reflection_executable_ivar_layout_emission_contract_id
          .empty() ||
      inputs
          .expected_storage_reflection_executable_synthesized_accessor_property_lowering_contract_id
          .empty() ||
      inputs.expected_bootstrap_live_registration_contract_id.empty() ||
      inputs.expected_bootstrap_live_restart_hardening_contract_id.empty() ||
      inputs.expected_bootstrap_replay_registered_images_symbol.empty() ||
      inputs.expected_bootstrap_reset_replay_state_snapshot_symbol.empty() ||
      inputs.expected_bootstrap_reset_for_testing_symbol.empty() ||
      inputs.local_driver_linker_flags.empty() ||
      inputs.direct_import_surface_artifact_paths.empty() ||
      inputs.imported_inputs.empty()) {
    error = "cross-module runtime link-plan artifact inputs are incomplete";
    return false;
  }
  // cleanup-unwind integration anchor: runnable cleanup/unwind
  // proofs stay toolchain-visible through the linker-response sidecar plus the
  // emitted runtime-support archive path that native executable probes consume.
  // runtime-fast-path-integration anchor: Part 9 keeps imported
  // direct-surface artifact paths visible in the cross-module link plan so the
  // runtime/cache lane can prove exactly which imported modules participate in
  // dispatch-boundary preservation before D002 widens the live fast path.
  // live-dispatch-fast-path anchor: imported direct-surface artifact paths feed the runtime cache seeding model once live direct/final/sealed fast paths are materialized after registration.
  if (inputs.direct_import_surface_artifact_paths.size() !=
      inputs.imported_inputs.size()) {
    error =
        "cross-module runtime link-plan direct import surface count does not "
        "match imported input count";
    return false;
  }
  if (inputs.local_storage_reflection_synthesized_accessor_entries !=
          inputs.local_storage_reflection_synthesized_getter_entries +
              inputs.local_storage_reflection_synthesized_setter_entries ||
      inputs.local_storage_reflection_ivar_layout_entries !=
          inputs.local_ivar_descriptor_count) {
    error =
        "cross-module runtime link-plan local storage/reflection preservation summary drifted from descriptor counts";
    return false;
  }
  if (inputs.local_block_ownership_invoke_trampoline_symbolized_sites >
          inputs.local_block_ownership_block_literal_sites ||
      inputs.local_block_ownership_copy_helper_symbolized_sites >
          inputs.local_block_ownership_copy_helper_required_sites ||
      inputs.local_block_ownership_dispose_helper_symbolized_sites >
          inputs.local_block_ownership_dispose_helper_required_sites ||
      inputs.local_block_ownership_escape_to_heap_sites >
          inputs.local_block_ownership_block_literal_sites) {
    error =
        "cross-module runtime link-plan local block-ownership preservation summary drifted from lowering counts";
    return false;
  }
  return true;
}
