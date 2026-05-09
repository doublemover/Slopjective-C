#include "io/objc3_cross_module_runtime_link_plan_document.h"

#include <sstream>
#include <vector>

#include "io/objc3_cross_module_imported_modules_document.h"
#include "io/objc3_cross_module_runtime_link_plan_inputs.h"
#include "io/objc3_cross_module_runtime_link_plan_ordering.h"
#include "io/objc3_cross_module_runtime_link_plan_sections.h"
#include "io/objc3_process_internal.h"

bool BuildObjc3CrossModuleRuntimeLinkPlanDocument(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    std::string &plan_json,
    std::string &linker_response_payload,
    std::string &error) {
  plan_json.clear();
  linker_response_payload.clear();
  error.clear();

  if (!TryValidateObjc3CrossModuleRuntimeLinkPlanArtifactInputs(inputs,
                                                               error)) {
    return false;
  }

  const std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput>
      imported_inputs =
          BuildOrderedObjc3CrossModuleRuntimeLinkPlanImportedInputs(
              inputs.imported_inputs);

  Objc3CrossModuleRuntimeLinkPlanSections sections;
  if (!BuildObjc3CrossModuleRuntimeLinkPlanSections(inputs,
                                                    imported_inputs,
                                                    sections,
                                                    error)) {
    return false;
  }

  const std::string imported_modules_json =
      BuildObjc3CrossModuleImportedModulesJson(imported_inputs);

  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \"" << EscapeJsonString(inputs.contract_id)
      << "\",\n"
      << "  \"source_orchestration_contract_id\": \""
      << EscapeJsonString(inputs.source_orchestration_contract_id) << "\",\n"
      << "  \"import_surface_contract_id\": \""
      << EscapeJsonString(inputs.import_surface_contract_id) << "\",\n"
      << "  \"registration_manifest_contract_id\": \""
      << EscapeJsonString(inputs.registration_manifest_contract_id)
      << "\",\n"
      << "  \"payload_model\": \"" << EscapeJsonString(inputs.payload_model)
      << "\",\n"
      << "  \"artifact\": \"" << EscapeJsonString(inputs.artifact_relative_path)
      << "\",\n"
      << "  \"linker_response_artifact\": \""
      << EscapeJsonString(inputs.linker_response_artifact_relative_path)
      << "\",\n"
      << "  \"authority_model\": \""
      << EscapeJsonString(inputs.authority_model) << "\",\n"
      << "  \"packaging_model\": \""
      << EscapeJsonString(inputs.packaging_model) << "\",\n"
      << "  \"registration_scope_model\": \""
      << EscapeJsonString(inputs.registration_scope_model) << "\",\n"
      << "  \"link_object_order_model\": \""
      << EscapeJsonString(inputs.link_object_order_model) << "\",\n"
      << "  \"expected_error_handling_contract_id\": \""
      << EscapeJsonString(inputs.expected_error_handling_contract_id) << "\",\n"
      << "  \"expected_error_handling_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_error_handling_source_contract_id)
      << "\",\n"
      << "  \"expected_concurrency_actor_contract_id\": \""
      << EscapeJsonString(inputs.expected_concurrency_actor_contract_id)
      << "\",\n"
      << "  \"expected_concurrency_actor_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_concurrency_actor_source_contract_id)
      << "\",\n"
      << "  \"expected_interop_ffi_contract_id\": \""
      << EscapeJsonString(inputs.expected_interop_ffi_contract_id) << "\",\n"
      << "  \"expected_interop_ffi_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_interop_ffi_source_contract_id)
      << "\",\n"
      << "  \"expected_interop_ffi_preservation_contract_id\": \""
      << EscapeJsonString(inputs.expected_interop_ffi_preservation_contract_id)
      << "\",\n"
      << "  \"expected_interop_header_module_bridge_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_interop_header_module_bridge_contract_id)
      << "\",\n"
      << "  \"expected_interop_header_module_bridge_source_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_interop_header_module_bridge_source_contract_id)
      << "\",\n"
      << "  \"expected_interop_header_module_bridge_preservation_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_interop_header_module_bridge_preservation_contract_id)
      << "\",\n"
      << "  \"expected_interop_bridge_header_artifact_relative_path\": \""
      << EscapeJsonString(
             inputs.expected_interop_bridge_header_artifact_relative_path)
      << "\",\n"
      << "  \"expected_interop_bridge_module_artifact_relative_path\": \""
      << EscapeJsonString(
             inputs.expected_interop_bridge_module_artifact_relative_path)
      << "\",\n"
      << "  \"expected_interop_bridge_artifact_relative_path\": \""
      << EscapeJsonString(inputs.expected_interop_bridge_artifact_relative_path)
      << "\",\n"
      << "  \"expected_metaprogramming_host_cache_contract_id\": \""
      << EscapeJsonString(inputs.expected_metaprogramming_host_cache_contract_id)
      << "\",\n"
      << "  \"expected_metaprogramming_host_cache_source_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_metaprogramming_host_cache_source_contract_id)
      << "\",\n"
      << "  \"expected_metaprogramming_host_cache_executable_relative_path\": \""
      << EscapeJsonString(
             inputs.expected_metaprogramming_host_cache_executable_relative_path)
      << "\",\n"
      << "  \"expected_metaprogramming_host_cache_root_relative_path\": \""
      << EscapeJsonString(
             inputs.expected_metaprogramming_host_cache_root_relative_path)
      << "\",\n"
      << "  \"expected_block_ownership_contract_id\": \""
      << EscapeJsonString(inputs.expected_block_ownership_contract_id)
      << "\",\n"
      << "  \"expected_block_ownership_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_block_ownership_source_contract_id)
      << "\",\n"
      << "  \"expected_block_ownership_object_invoke_thunk_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_object_invoke_thunk_lowering_contract_id)
      << "\",\n"
      << "  \"expected_block_ownership_byref_helper_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_block_ownership_byref_helper_lowering_contract_id)
      << "\",\n"
      << "  \"expected_block_ownership_escape_runtime_hook_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_escape_runtime_hook_lowering_contract_id)
      << "\",\n"
      << "  \"expected_block_ownership_runtime_support_library_link_wiring_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_runtime_support_library_link_wiring_contract_id)
      << "\",\n"
      << "  \"module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(sections.module_names_lexicographic,
                                      "    ")
      << ",\n"
      << "  \"module_image_count\": "
      << sections.module_names_lexicographic.size() << ",\n"
      << "  \"direct_import_input_count\": "
      << sections.direct_import_surface_artifact_paths.size() << ",\n"
      << "  \"error_handling_imported_module_count\": "
      << sections.imported_error_handling_module_names_lexicographic.size()
      << ",\n"
      << "  \"concurrency_actor_imported_module_count\": "
      << sections.imported_concurrency_actor_module_names_lexicographic.size()
      << ",\n"
      << "  \"interop_ffi_imported_module_count\": "
      << sections.imported_interop_ffi_module_names_lexicographic.size()
      << ",\n"
      << "  \"interop_header_module_bridge_imported_module_count\": "
      << sections
             .imported_interop_header_module_bridge_module_names_lexicographic
             .size()
      << ",\n"
      << "  \"metaprogramming_host_cache_imported_module_count\": "
      << sections.imported_metaprogramming_host_cache_module_names_lexicographic
             .size()
      << ",\n"
      << "  \"direct_import_surface_artifact_paths\": "
      << BuildIndentedStringArrayJson(
             sections.direct_import_surface_artifact_paths, "    ")
      << ",\n"
      << "  \"error_handling_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections.imported_error_handling_module_names_lexicographic,
             "    ")
      << ",\n"
      << "  \"concurrency_actor_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections.imported_concurrency_actor_module_names_lexicographic,
             "    ")
      << ",\n"
      << "  \"interop_ffi_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections.imported_interop_ffi_module_names_lexicographic, "    ")
      << ",\n"
      << "  \"interop_header_module_bridge_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections
                 .imported_interop_header_module_bridge_module_names_lexicographic,
             "    ")
      << ",\n"
      << "  \"metaprogramming_host_cache_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             sections
                 .imported_metaprogramming_host_cache_module_names_lexicographic,
             "    ")
      << ",\n"
      << "  \"error_handling_cross_module_preservation_ready\": "
      << (!sections.imported_error_handling_module_names_lexicographic.empty()
              ? "true"
              : "false")
      << ",\n"
      << "  \"concurrency_actor_cross_module_isolation_ready\": "
      << (!sections.imported_concurrency_actor_module_names_lexicographic
               .empty()
              ? "true"
              : "false")
      << ",\n"
      << "  \"interop_ffi_cross_module_packaging_ready\": "
      << (!sections.imported_interop_ffi_module_names_lexicographic.empty()
              ? "true"
              : "false")
      << ",\n"
      << "  \"interop_header_module_bridge_cross_module_packaging_ready\": "
      << (!sections
               .imported_interop_header_module_bridge_module_names_lexicographic
               .empty()
              ? "true"
              : "false")
      << ",\n"
      << "  \"metaprogramming_host_cache_cross_module_preservation_ready\": "
      << (!sections
               .imported_metaprogramming_host_cache_module_names_lexicographic
               .empty()
              ? "true"
              : "false")
      << ",\n"
      << "  \"bootstrap_live_registration_contract_id\": \""
      << EscapeJsonString(inputs.expected_bootstrap_live_registration_contract_id)
      << "\",\n"
      << "  \"bootstrap_live_restart_hardening_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_bootstrap_live_restart_hardening_contract_id)
      << "\",\n"
      << "  \"bootstrap_replay_registered_images_symbol\": \""
      << EscapeJsonString(
             inputs.expected_bootstrap_replay_registered_images_symbol)
      << "\",\n"
      << "  \"bootstrap_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.expected_bootstrap_reset_replay_state_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_reset_for_testing_symbol\": \""
      << EscapeJsonString(inputs.expected_bootstrap_reset_for_testing_symbol)
      << "\",\n"
      << "  \"runtime_cross_module_realized_metadata_replay_preservation_surface_contract_id\": \""
      << "objc3c.runtime.cross.module.realized.metadata.replay.preservation.surface.v1"
      << "\",\n"
      << "  \"runtime_object_model_realization_source_surface_contract_id\": \""
      << "objc3c.runtime.object.model.realization.source.surface.v1"
      << "\",\n"
      << "  \"runtime_realization_lowering_reflection_artifact_surface_contract_id\": \""
      << "objc3c.runtime.realization.lowering.reflection.artifact.surface.v1"
      << "\",\n"
      << "  \"runtime_dispatch_table_reflection_record_lowering_surface_contract_id\": \""
      << "objc3c.runtime.dispatch.table.reflection.record.lowering.surface.v1"
      << "\",\n"
      << "  \"runtime_cross_module_block_ownership_artifact_preservation_surface_contract_id\": \""
      << EscapeJsonString(inputs.expected_block_ownership_contract_id)
      << "\",\n"
      << "  \"runtime_block_arc_lowering_helper_surface_contract_id\": \""
      << EscapeJsonString(inputs.expected_block_ownership_source_contract_id)
      << "\",\n"
      << "  \"block_object_invoke_thunk_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_object_invoke_thunk_lowering_contract_id)
      << "\",\n"
      << "  \"block_byref_helper_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_block_ownership_byref_helper_lowering_contract_id)
      << "\",\n"
      << "  \"block_escape_runtime_hook_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_escape_runtime_hook_lowering_contract_id)
      << "\",\n"
      << "  \"block_runtime_support_library_link_wiring_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_block_ownership_runtime_support_library_link_wiring_contract_id)
      << "\",\n"
      << "  \"runtime_cross_module_storage_reflection_artifact_preservation_surface_contract_id\": \""
      << EscapeJsonString(inputs.expected_storage_reflection_contract_id)
      << "\",\n"
      << "  \"runtime_property_ivar_storage_accessor_source_surface_contract_id\": \""
      << EscapeJsonString(inputs.expected_storage_reflection_source_contract_id)
      << "\",\n"
      << "  \"dispatch_and_synthesized_accessor_lowering_surface_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id)
      << "\",\n"
      << "  \"executable_property_accessor_layout_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_storage_reflection_executable_property_accessor_layout_lowering_contract_id)
      << "\",\n"
      << "  \"executable_ivar_layout_emission_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_storage_reflection_executable_ivar_layout_emission_contract_id)
      << "\",\n"
      << "  \"executable_synthesized_accessor_property_lowering_contract_id\": \""
      << EscapeJsonString(
             inputs
                 .expected_storage_reflection_executable_synthesized_accessor_property_lowering_contract_id)
      << "\",\n"
      << "  \"realized_metadata_replay_preservation_model\": \""
      << "cross-module-link-plan-preserves-local-and-imported-realized-metadata-descriptor-counts-identities-and-reset-replay-readiness-from-runtime-registration-manifests"
      << "\",\n"
      << "  \"block_ownership_artifact_preservation_model\": \""
      << "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-block-ownership-lowering-helper-and-runtime-link-facts-beyond-local-ir-object-emission"
      << "\",\n"
      << "  \"storage_reflection_artifact_preservation_model\": \""
      << "provider-and-consumer-runtime-import-surfaces-and-cross-module-link-plans-preserve-property-ivar-accessor-layout-and-runtime-helper-facts-beyond-local-ir-object-emission"
      << "\",\n"
      << "  \"imported_live_registration_replay_ready\": true,\n"
      << "  \"imported_live_restart_hardening_ready\": true,\n"
      << "  \"block_ownership_cross_module_preservation_ready\": true,\n"
      << "  \"storage_reflection_cross_module_preservation_ready\": true,\n"
      << "  \"module_image_count\": " << imported_inputs.size() + 1 << ",\n"
      << "  \"direct_import_input_count\": " << imported_inputs.size() << ",\n"
      << "  \"local_class_descriptor_count\": "
      << inputs.local_class_descriptor_count << ",\n"
      << "  \"local_protocol_descriptor_count\": "
      << inputs.local_protocol_descriptor_count << ",\n"
      << "  \"local_category_descriptor_count\": "
      << inputs.local_category_descriptor_count << ",\n"
      << "  \"local_property_descriptor_count\": "
      << inputs.local_property_descriptor_count << ",\n"
      << "  \"local_ivar_descriptor_count\": "
      << inputs.local_ivar_descriptor_count << ",\n"
      << "  \"local_total_descriptor_count\": "
      << inputs.local_total_descriptor_count << ",\n"
      << "  \"imported_class_descriptor_count\": "
      << sections.imported_class_descriptor_count << ",\n"
      << "  \"imported_protocol_descriptor_count\": "
      << sections.imported_protocol_descriptor_count << ",\n"
      << "  \"imported_category_descriptor_count\": "
      << sections.imported_category_descriptor_count << ",\n"
      << "  \"imported_property_descriptor_count\": "
      << sections.imported_property_descriptor_count << ",\n"
      << "  \"imported_ivar_descriptor_count\": "
      << sections.imported_ivar_descriptor_count << ",\n"
      << "  \"imported_total_descriptor_count\": "
      << sections.imported_total_descriptor_count << ",\n"
      << "  \"transitive_class_descriptor_count\": "
      << inputs.local_class_descriptor_count +
             sections.imported_class_descriptor_count
      << ",\n"
      << "  \"transitive_protocol_descriptor_count\": "
      << inputs.local_protocol_descriptor_count +
             sections.imported_protocol_descriptor_count
      << ",\n"
      << "  \"transitive_category_descriptor_count\": "
      << inputs.local_category_descriptor_count +
             sections.imported_category_descriptor_count
      << ",\n"
      << "  \"transitive_property_descriptor_count\": "
      << inputs.local_property_descriptor_count +
             sections.imported_property_descriptor_count
      << ",\n"
      << "  \"transitive_ivar_descriptor_count\": "
      << inputs.local_ivar_descriptor_count +
             sections.imported_ivar_descriptor_count
      << ",\n"
      << "  \"transitive_total_descriptor_count\": "
      << inputs.local_total_descriptor_count +
             sections.imported_total_descriptor_count
      << ",\n"
      << "  \"local_block_ownership_block_literal_sites\": "
      << inputs.local_block_ownership_block_literal_sites << ",\n"
      << "  \"local_block_ownership_invoke_trampoline_symbolized_sites\": "
      << inputs.local_block_ownership_invoke_trampoline_symbolized_sites
      << ",\n"
      << "  \"local_block_ownership_copy_helper_required_sites\": "
      << inputs.local_block_ownership_copy_helper_required_sites << ",\n"
      << "  \"local_block_ownership_dispose_helper_required_sites\": "
      << inputs.local_block_ownership_dispose_helper_required_sites << ",\n"
      << "  \"local_block_ownership_copy_helper_symbolized_sites\": "
      << inputs.local_block_ownership_copy_helper_symbolized_sites << ",\n"
      << "  \"local_block_ownership_dispose_helper_symbolized_sites\": "
      << inputs.local_block_ownership_dispose_helper_symbolized_sites
      << ",\n"
      << "  \"local_block_ownership_escape_to_heap_sites\": "
      << inputs.local_block_ownership_escape_to_heap_sites << ",\n"
      << "  \"local_block_ownership_byref_layout_symbolized_sites\": "
      << inputs.local_block_ownership_byref_layout_symbolized_sites
      << ",\n"
      << "  \"imported_block_ownership_block_literal_sites\": "
      << sections.imported_block_ownership_block_literal_sites << ",\n"
      << "  \"imported_block_ownership_invoke_trampoline_symbolized_sites\": "
      << sections.imported_block_ownership_invoke_trampoline_symbolized_sites
      << ",\n"
      << "  \"imported_block_ownership_copy_helper_required_sites\": "
      << sections.imported_block_ownership_copy_helper_required_sites << ",\n"
      << "  \"imported_block_ownership_dispose_helper_required_sites\": "
      << sections.imported_block_ownership_dispose_helper_required_sites
      << ",\n"
      << "  \"imported_block_ownership_copy_helper_symbolized_sites\": "
      << sections.imported_block_ownership_copy_helper_symbolized_sites
      << ",\n"
      << "  \"imported_block_ownership_dispose_helper_symbolized_sites\": "
      << sections.imported_block_ownership_dispose_helper_symbolized_sites
      << ",\n"
      << "  \"imported_block_ownership_escape_to_heap_sites\": "
      << sections.imported_block_ownership_escape_to_heap_sites << ",\n"
      << "  \"imported_block_ownership_byref_layout_symbolized_sites\": "
      << sections.imported_block_ownership_byref_layout_symbolized_sites
      << ",\n"
      << "  \"transitive_block_ownership_block_literal_sites\": "
      << inputs.local_block_ownership_block_literal_sites +
             sections.imported_block_ownership_block_literal_sites
      << ",\n"
      << "  \"transitive_block_ownership_invoke_trampoline_symbolized_sites\": "
      << inputs.local_block_ownership_invoke_trampoline_symbolized_sites +
             sections
                 .imported_block_ownership_invoke_trampoline_symbolized_sites
      << ",\n"
      << "  \"transitive_block_ownership_copy_helper_required_sites\": "
      << inputs.local_block_ownership_copy_helper_required_sites +
             sections.imported_block_ownership_copy_helper_required_sites
      << ",\n"
      << "  \"transitive_block_ownership_dispose_helper_required_sites\": "
      << inputs.local_block_ownership_dispose_helper_required_sites +
             sections.imported_block_ownership_dispose_helper_required_sites
      << ",\n"
      << "  \"transitive_block_ownership_copy_helper_symbolized_sites\": "
      << inputs.local_block_ownership_copy_helper_symbolized_sites +
             sections.imported_block_ownership_copy_helper_symbolized_sites
      << ",\n"
      << "  \"transitive_block_ownership_dispose_helper_symbolized_sites\": "
      << inputs.local_block_ownership_dispose_helper_symbolized_sites +
             sections.imported_block_ownership_dispose_helper_symbolized_sites
      << ",\n"
      << "  \"transitive_block_ownership_escape_to_heap_sites\": "
      << inputs.local_block_ownership_escape_to_heap_sites +
             sections.imported_block_ownership_escape_to_heap_sites
      << ",\n"
      << "  \"transitive_block_ownership_byref_layout_symbolized_sites\": "
      << inputs.local_block_ownership_byref_layout_symbolized_sites +
             sections.imported_block_ownership_byref_layout_symbolized_sites
      << ",\n"
      << "  \"local_storage_reflection_implementation_owned_property_entries\": "
      << inputs.local_storage_reflection_implementation_owned_property_entries
      << ",\n"
      << "  \"local_storage_reflection_synthesized_accessor_owner_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_owner_entries
      << ",\n"
      << "  \"local_storage_reflection_synthesized_getter_entries\": "
      << inputs.local_storage_reflection_synthesized_getter_entries
      << ",\n"
      << "  \"local_storage_reflection_synthesized_setter_entries\": "
      << inputs.local_storage_reflection_synthesized_setter_entries
      << ",\n"
      << "  \"local_storage_reflection_synthesized_accessor_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_entries
      << ",\n"
      << "  \"local_storage_reflection_current_property_read_entries\": "
      << inputs.local_storage_reflection_current_property_read_entries
      << ",\n"
      << "  \"local_storage_reflection_current_property_write_entries\": "
      << inputs.local_storage_reflection_current_property_write_entries
      << ",\n"
      << "  \"local_storage_reflection_current_property_exchange_entries\": "
      << inputs.local_storage_reflection_current_property_exchange_entries
      << ",\n"
      << "  \"local_storage_reflection_weak_current_property_load_entries\": "
      << inputs.local_storage_reflection_weak_current_property_load_entries
      << ",\n"
      << "  \"local_storage_reflection_weak_current_property_store_entries\": "
      << inputs.local_storage_reflection_weak_current_property_store_entries
      << ",\n"
      << "  \"local_storage_reflection_ivar_layout_entries\": "
      << inputs.local_storage_reflection_ivar_layout_entries << ",\n"
      << "  \"local_storage_reflection_ivar_layout_owner_entries\": "
      << inputs.local_storage_reflection_ivar_layout_owner_entries << ",\n"
      << "  \"imported_storage_reflection_implementation_owned_property_entries\": "
      << sections
             .imported_storage_reflection_implementation_owned_property_entries
      << ",\n"
      << "  \"imported_storage_reflection_synthesized_accessor_owner_entries\": "
      << sections
             .imported_storage_reflection_synthesized_accessor_owner_entries
      << ",\n"
      << "  \"imported_storage_reflection_synthesized_getter_entries\": "
      << sections.imported_storage_reflection_synthesized_getter_entries
      << ",\n"
      << "  \"imported_storage_reflection_synthesized_setter_entries\": "
      << sections.imported_storage_reflection_synthesized_setter_entries
      << ",\n"
      << "  \"imported_storage_reflection_synthesized_accessor_entries\": "
      << sections.imported_storage_reflection_synthesized_accessor_entries
      << ",\n"
      << "  \"imported_storage_reflection_current_property_read_entries\": "
      << sections.imported_storage_reflection_current_property_read_entries
      << ",\n"
      << "  \"imported_storage_reflection_current_property_write_entries\": "
      << sections.imported_storage_reflection_current_property_write_entries
      << ",\n"
      << "  \"imported_storage_reflection_current_property_exchange_entries\": "
      << sections.imported_storage_reflection_current_property_exchange_entries
      << ",\n"
      << "  \"imported_storage_reflection_weak_current_property_load_entries\": "
      << sections.imported_storage_reflection_weak_current_property_load_entries
      << ",\n"
      << "  \"imported_storage_reflection_weak_current_property_store_entries\": "
      << sections.imported_storage_reflection_weak_current_property_store_entries
      << ",\n"
      << "  \"imported_storage_reflection_ivar_layout_entries\": "
      << sections.imported_storage_reflection_ivar_layout_entries << ",\n"
      << "  \"imported_storage_reflection_ivar_layout_owner_entries\": "
      << sections.imported_storage_reflection_ivar_layout_owner_entries
      << ",\n"
      << "  \"transitive_storage_reflection_implementation_owned_property_entries\": "
      << inputs.local_storage_reflection_implementation_owned_property_entries +
             sections
                 .imported_storage_reflection_implementation_owned_property_entries
      << ",\n"
      << "  \"transitive_storage_reflection_synthesized_accessor_owner_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_owner_entries +
             sections
                 .imported_storage_reflection_synthesized_accessor_owner_entries
      << ",\n"
      << "  \"transitive_storage_reflection_synthesized_getter_entries\": "
      << inputs.local_storage_reflection_synthesized_getter_entries +
             sections.imported_storage_reflection_synthesized_getter_entries
      << ",\n"
      << "  \"transitive_storage_reflection_synthesized_setter_entries\": "
      << inputs.local_storage_reflection_synthesized_setter_entries +
             sections.imported_storage_reflection_synthesized_setter_entries
      << ",\n"
      << "  \"transitive_storage_reflection_synthesized_accessor_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_entries +
             sections.imported_storage_reflection_synthesized_accessor_entries
      << ",\n"
      << "  \"transitive_storage_reflection_current_property_read_entries\": "
      << inputs.local_storage_reflection_current_property_read_entries +
             sections.imported_storage_reflection_current_property_read_entries
      << ",\n"
      << "  \"transitive_storage_reflection_current_property_write_entries\": "
      << inputs.local_storage_reflection_current_property_write_entries +
             sections.imported_storage_reflection_current_property_write_entries
      << ",\n"
      << "  \"transitive_storage_reflection_current_property_exchange_entries\": "
      << inputs.local_storage_reflection_current_property_exchange_entries +
             sections
                 .imported_storage_reflection_current_property_exchange_entries
      << ",\n"
      << "  \"transitive_storage_reflection_weak_current_property_load_entries\": "
      << inputs.local_storage_reflection_weak_current_property_load_entries +
             sections
                 .imported_storage_reflection_weak_current_property_load_entries
      << ",\n"
      << "  \"transitive_storage_reflection_weak_current_property_store_entries\": "
      << inputs.local_storage_reflection_weak_current_property_store_entries +
             sections
                 .imported_storage_reflection_weak_current_property_store_entries
      << ",\n"
      << "  \"transitive_storage_reflection_ivar_layout_entries\": "
      << inputs.local_storage_reflection_ivar_layout_entries +
             sections.imported_storage_reflection_ivar_layout_entries
      << ",\n"
      << "  \"transitive_storage_reflection_ivar_layout_owner_entries\": "
      << inputs.local_storage_reflection_ivar_layout_owner_entries +
             sections.imported_storage_reflection_ivar_layout_owner_entries
      << ",\n"
      << "  \"cleanup_unwind_runtime_link_model\": "
      << "\"linker-response-plus-runtime-support-archive-sidecars-provide-runnable-cleanup-executable-link-inputs\",\n"
      << "  \"runtime_support_library_archive_relative_path\": \""
      << EscapeJsonString(inputs.runtime_support_library_archive_relative_path)
      << "\",\n"
      << "  \"object_format\": \"" << EscapeJsonString(inputs.object_format)
      << "\",\n"
      << "  \"local_module\": {\n"
      << "    \"module_name\": \""
      << EscapeJsonString(inputs.local_module_name) << "\",\n"
      << "    \"import_surface_artifact_relative_path\": \""
      << EscapeJsonString(inputs.local_import_surface_artifact_relative_path)
      << "\",\n"
      << "    \"registration_manifest_artifact_relative_path\": \""
      << EscapeJsonString(
             inputs.local_registration_manifest_artifact_relative_path)
      << "\",\n"
      << "    \"object_artifact_relative_path\": \""
      << EscapeJsonString(inputs.local_object_artifact_relative_path)
      << "\",\n"
      << "    \"translation_unit_identity_model\": \""
      << EscapeJsonString(inputs.local_translation_unit_identity_model)
      << "\",\n"
      << "    \"translation_unit_identity_key\": \""
      << EscapeJsonString(inputs.local_translation_unit_identity_key)
      << "\",\n"
      << "    \"translation_unit_registration_order_ordinal\": "
      << inputs.local_translation_unit_registration_order_ordinal << ",\n"
      << "    \"class_descriptor_count\": "
      << inputs.local_class_descriptor_count << ",\n"
      << "    \"protocol_descriptor_count\": "
      << inputs.local_protocol_descriptor_count << ",\n"
      << "    \"category_descriptor_count\": "
      << inputs.local_category_descriptor_count << ",\n"
      << "    \"property_descriptor_count\": "
      << inputs.local_property_descriptor_count << ",\n"
      << "    \"ivar_descriptor_count\": "
      << inputs.local_ivar_descriptor_count << ",\n"
      << "    \"total_descriptor_count\": "
      << inputs.local_total_descriptor_count << ",\n"
      << "    \"storage_reflection_implementation_owned_property_entries\": "
      << inputs.local_storage_reflection_implementation_owned_property_entries
      << ",\n"
      << "    \"storage_reflection_synthesized_accessor_owner_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_owner_entries
      << ",\n"
      << "    \"storage_reflection_synthesized_getter_entries\": "
      << inputs.local_storage_reflection_synthesized_getter_entries
      << ",\n"
      << "    \"storage_reflection_synthesized_setter_entries\": "
      << inputs.local_storage_reflection_synthesized_setter_entries
      << ",\n"
      << "    \"storage_reflection_synthesized_accessor_entries\": "
      << inputs.local_storage_reflection_synthesized_accessor_entries
      << ",\n"
      << "    \"storage_reflection_current_property_read_entries\": "
      << inputs.local_storage_reflection_current_property_read_entries
      << ",\n"
      << "    \"storage_reflection_current_property_write_entries\": "
      << inputs.local_storage_reflection_current_property_write_entries
      << ",\n"
      << "    \"storage_reflection_current_property_exchange_entries\": "
      << inputs.local_storage_reflection_current_property_exchange_entries
      << ",\n"
      << "    \"storage_reflection_weak_current_property_load_entries\": "
      << inputs.local_storage_reflection_weak_current_property_load_entries
      << ",\n"
      << "    \"storage_reflection_weak_current_property_store_entries\": "
      << inputs.local_storage_reflection_weak_current_property_store_entries
      << ",\n"
      << "    \"storage_reflection_ivar_layout_entries\": "
      << inputs.local_storage_reflection_ivar_layout_entries << ",\n"
      << "    \"storage_reflection_ivar_layout_owner_entries\": "
      << inputs.local_storage_reflection_ivar_layout_owner_entries << ",\n"
      << "    \"driver_linker_flags\": "
      << BuildIndentedStringArrayJson(inputs.local_driver_linker_flags,
                                      "      ")
      << "\n"
      << "  },\n"
      << "  \"imported_modules\": " << imported_modules_json << ",\n"
      << "  \"link_object_artifacts\": "
      << BuildIndentedStringArrayJson(sections.ordered_link_object_artifacts,
                                      "    ")
      << ",\n"
      << "  \"driver_linker_flags\": "
      << BuildIndentedStringArrayJson(sections.merged_driver_linker_flags,
                                      "    ")
      << ",\n"
      << "  \"ready\": true\n"
      << "}\n";
  plan_json = out.str();

  linker_response_payload =
      BuildObjc3CrossModuleRuntimeLinkerResponsePayload(
          sections.merged_driver_linker_flags);
  return true;
}
