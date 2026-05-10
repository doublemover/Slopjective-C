#include "io/objc3_cross_module_imported_modules_document_record_storage_sections.h"

#include <ostream>

#include "io/objc3_json.h"
#include "io/objc3_process_json_helpers.h"

using objc3::io::EscapeJsonString;

void EmitObjc3CrossModuleImportedModuleRecordStorageSectionsJson(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input) {
  out << "      \"block_ownership_artifact_preservation_present\": "
      << (imported_input.block_ownership_artifact_preservation_present
              ? "true"
              : "false")
      << ",\n"
      << "      \"block_ownership_runtime_import_artifact_ready\": "
      << (imported_input.block_ownership_runtime_import_artifact_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"block_ownership_separate_compilation_preservation_ready\": "
      << (imported_input
                  .block_ownership_separate_compilation_preservation_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"block_ownership_runtime_support_library_link_wiring_ready\": "
      << (imported_input
                  .block_ownership_runtime_support_library_link_wiring_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"block_ownership_deterministic\": "
      << (imported_input.block_ownership_deterministic ? "true"
                                                       : "false")
      << ",\n"
      << "      \"block_ownership_contract_id\": \""
      << EscapeJsonString(imported_input.block_ownership_contract_id)
      << "\",\n"
      << "      \"block_ownership_source_contract_id\": \""
      << EscapeJsonString(imported_input.block_ownership_source_contract_id)
      << "\",\n"
      << "      \"block_ownership_object_invoke_thunk_lowering_contract_id\": \""
      << EscapeJsonString(
             imported_input.block_ownership_object_invoke_thunk_lowering_contract_id)
      << "\",\n"
      << "      \"block_ownership_byref_helper_lowering_contract_id\": \""
      << EscapeJsonString(
             imported_input.block_ownership_byref_helper_lowering_contract_id)
      << "\",\n"
      << "      \"block_ownership_escape_runtime_hook_lowering_contract_id\": \""
      << EscapeJsonString(
             imported_input.block_ownership_escape_runtime_hook_lowering_contract_id)
      << "\",\n"
      << "      \"block_ownership_runtime_support_library_link_wiring_contract_id\": \""
      << EscapeJsonString(
             imported_input
                 .block_ownership_runtime_support_library_link_wiring_contract_id)
      << "\",\n"
      << "      \"block_ownership_replay_key\": \""
      << EscapeJsonString(imported_input.block_ownership_replay_key)
      << "\",\n"
      << "      \"block_ownership_local_block_literal_sites\": "
      << imported_input.block_ownership_local_block_literal_sites << ",\n"
      << "      \"block_ownership_local_invoke_trampoline_symbolized_sites\": "
      << imported_input.block_ownership_local_invoke_trampoline_symbolized_sites
      << ",\n"
      << "      \"block_ownership_local_copy_helper_required_sites\": "
      << imported_input.block_ownership_local_copy_helper_required_sites
      << ",\n"
      << "      \"block_ownership_local_dispose_helper_required_sites\": "
      << imported_input.block_ownership_local_dispose_helper_required_sites
      << ",\n"
      << "      \"block_ownership_local_copy_helper_symbolized_sites\": "
      << imported_input.block_ownership_local_copy_helper_symbolized_sites
      << ",\n"
      << "      \"block_ownership_local_dispose_helper_symbolized_sites\": "
      << imported_input.block_ownership_local_dispose_helper_symbolized_sites
      << ",\n"
      << "      \"block_ownership_local_escape_to_heap_sites\": "
      << imported_input.block_ownership_local_escape_to_heap_sites << ",\n"
      << "      \"block_ownership_local_byref_layout_symbolized_sites\": "
      << imported_input.block_ownership_local_byref_layout_symbolized_sites
      << ",\n"
      << "      \"storage_reflection_artifact_preservation_present\": "
      << (imported_input.storage_reflection_artifact_preservation_present
              ? "true"
              : "false")
      << ",\n"
      << "      \"storage_reflection_runtime_import_artifact_ready\": "
      << (imported_input.storage_reflection_runtime_import_artifact_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"storage_reflection_separate_compilation_preservation_ready\": "
      << (imported_input.storage_reflection_separate_compilation_preservation_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"storage_reflection_deterministic\": "
      << (imported_input.storage_reflection_deterministic ? "true"
                                                          : "false")
      << ",\n"
      << "      \"storage_reflection_contract_id\": \""
      << EscapeJsonString(imported_input.storage_reflection_contract_id)
      << "\",\n"
      << "      \"storage_reflection_source_contract_id\": \""
      << EscapeJsonString(imported_input.storage_reflection_source_contract_id)
      << "\",\n"
      << "      \"storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id\": \""
      << EscapeJsonString(
             imported_input
                 .storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id)
      << "\",\n"
      << "      \"storage_reflection_executable_property_accessor_layout_lowering_contract_id\": \""
      << EscapeJsonString(
             imported_input
                 .storage_reflection_executable_property_accessor_layout_lowering_contract_id)
      << "\",\n"
      << "      \"storage_reflection_executable_ivar_layout_emission_contract_id\": \""
      << EscapeJsonString(
             imported_input
                 .storage_reflection_executable_ivar_layout_emission_contract_id)
      << "\",\n"
      << "      \"storage_reflection_executable_synthesized_accessor_property_lowering_contract_id\": \""
      << EscapeJsonString(
             imported_input
                 .storage_reflection_executable_synthesized_accessor_property_lowering_contract_id)
      << "\",\n"
      << "      \"storage_reflection_replay_key\": \""
      << EscapeJsonString(imported_input.storage_reflection_replay_key)
      << "\",\n"
      << "      \"storage_reflection_local_property_descriptor_count\": "
      << imported_input.storage_reflection_local_property_descriptor_count
      << ",\n"
      << "      \"storage_reflection_local_ivar_descriptor_count\": "
      << imported_input.storage_reflection_local_ivar_descriptor_count
      << ",\n"
      << "      \"storage_reflection_implementation_owned_property_entries\": "
      << imported_input.storage_reflection_implementation_owned_property_entries
      << ",\n"
      << "      \"storage_reflection_synthesized_accessor_owner_entries\": "
      << imported_input.storage_reflection_synthesized_accessor_owner_entries
      << ",\n"
      << "      \"storage_reflection_synthesized_getter_entries\": "
      << imported_input.storage_reflection_synthesized_getter_entries
      << ",\n"
      << "      \"storage_reflection_synthesized_setter_entries\": "
      << imported_input.storage_reflection_synthesized_setter_entries
      << ",\n"
      << "      \"storage_reflection_synthesized_accessor_entries\": "
      << imported_input.storage_reflection_synthesized_accessor_entries
      << ",\n"
      << "      \"storage_reflection_current_property_read_entries\": "
      << imported_input.storage_reflection_current_property_read_entries
      << ",\n"
      << "      \"storage_reflection_current_property_write_entries\": "
      << imported_input.storage_reflection_current_property_write_entries
      << ",\n"
      << "      \"storage_reflection_current_property_exchange_entries\": "
      << imported_input.storage_reflection_current_property_exchange_entries
      << ",\n"
      << "      \"storage_reflection_weak_current_property_load_entries\": "
      << imported_input.storage_reflection_weak_current_property_load_entries
      << ",\n"
      << "      \"storage_reflection_weak_current_property_store_entries\": "
      << imported_input.storage_reflection_weak_current_property_store_entries
      << ",\n"
      << "      \"storage_reflection_ivar_layout_entries\": "
      << imported_input.storage_reflection_ivar_layout_entries
      << ",\n"
      << "      \"storage_reflection_ivar_layout_owner_entries\": "
      << imported_input.storage_reflection_ivar_layout_owner_entries
      << ",\n"
      << "      \"driver_linker_flags\": "
      << BuildIndentedStringArrayJson(imported_input.driver_linker_flags,
                                      "        ");
}
