#include "io/objc3_cross_module_imported_modules_document_records.h"

#include <ostream>

#include "io/objc3_json.h"
#include "io/objc3_process_json_helpers.h"

using objc3::io::EscapeJsonString;

void EmitObjc3CrossModuleImportedModuleRecordJson(
    std::ostream &out,
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input) {
  out << "    {\n"
      << "      \"module_name\": \""
      << EscapeJsonString(imported_input.module_name) << "\",\n"
      << "      \"import_surface_artifact_path\": \""
      << EscapeJsonString(imported_input.import_surface_artifact_path)
      << "\",\n"
      << "      \"registration_manifest_artifact_path\": \""
      << EscapeJsonString(imported_input.registration_manifest_artifact_path)
      << "\",\n"
      << "      \"object_artifact_path\": \""
      << EscapeJsonString(imported_input.object_artifact_path) << "\",\n"
      << "      \"discovery_artifact_path\": \""
      << EscapeJsonString(imported_input.discovery_artifact_path) << "\",\n"
      << "      \"linker_response_artifact_path\": \""
      << EscapeJsonString(imported_input.linker_response_artifact_path)
      << "\",\n"
      << "      \"translation_unit_identity_model\": \""
      << EscapeJsonString(imported_input.translation_unit_identity_model)
      << "\",\n"
      << "      \"translation_unit_identity_key\": \""
      << EscapeJsonString(imported_input.translation_unit_identity_key)
      << "\",\n"
      << "      \"translation_unit_registration_order_ordinal\": "
      << imported_input.translation_unit_registration_order_ordinal << ",\n"
      << "      \"class_descriptor_count\": "
      << imported_input.class_descriptor_count << ",\n"
      << "      \"protocol_descriptor_count\": "
      << imported_input.protocol_descriptor_count << ",\n"
      << "      \"category_descriptor_count\": "
      << imported_input.category_descriptor_count << ",\n"
      << "      \"property_descriptor_count\": "
      << imported_input.property_descriptor_count << ",\n"
      << "      \"ivar_descriptor_count\": "
      << imported_input.ivar_descriptor_count << ",\n"
      << "      \"total_descriptor_count\": "
      << imported_input.total_descriptor_count << ",\n"
      << "      \"object_format\": \""
      << EscapeJsonString(imported_input.object_format) << "\",\n"
      << "      \"runtime_support_library_archive_relative_path\": \""
      << EscapeJsonString(
             imported_input.runtime_support_library_archive_relative_path)
      << "\",\n"
      << "      \"ready_for_live_registration_discovery_replay\": "
      << (imported_input.ready_for_live_registration_discovery_replay
              ? "true"
              : "false")
      << ",\n"
      << "      \"ready_for_live_restart_hardening\": "
      << (imported_input.ready_for_live_restart_hardening ? "true"
                                                          : "false")
      << ",\n"
      << "      \"bootstrap_live_registration_contract_id\": \""
      << EscapeJsonString(imported_input.bootstrap_live_registration_contract_id)
      << "\",\n"
      << "      \"bootstrap_live_restart_hardening_contract_id\": \""
      << EscapeJsonString(
             imported_input.bootstrap_live_restart_hardening_contract_id)
      << "\",\n"
      << "      \"bootstrap_live_replay_registered_images_symbol\": \""
      << EscapeJsonString(
             imported_input.bootstrap_live_replay_registered_images_symbol)
      << "\",\n"
      << "      \"bootstrap_live_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             imported_input.bootstrap_live_reset_replay_state_snapshot_symbol)
      << "\",\n"
      << "      \"bootstrap_live_restart_reset_for_testing_symbol\": \""
      << EscapeJsonString(
             imported_input.bootstrap_live_restart_reset_for_testing_symbol)
      << "\",\n"
      << "      \"bootstrap_live_restart_replay_registered_images_symbol\": \""
      << EscapeJsonString(
             imported_input
                 .bootstrap_live_restart_replay_registered_images_symbol)
      << "\",\n"
      << "      \"bootstrap_live_restart_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             imported_input
                 .bootstrap_live_restart_reset_replay_state_snapshot_symbol)
      << "\",\n"
      << "      \"error_handling_result_and_bridging_artifact_replay_present\": "
      << (imported_input.error_handling_result_and_bridging_artifact_replay_present
              ? "true"
              : "false")
      << ",\n"
      << "      \"error_handling_binary_artifact_replay_ready\": "
      << (imported_input.error_handling_binary_artifact_replay_ready ? "true"
                                                                     : "false")
      << ",\n"
      << "      \"error_handling_runtime_import_artifact_ready\": "
      << (imported_input.error_handling_runtime_import_artifact_ready ? "true"
                                                                     : "false")
      << ",\n"
      << "      \"error_handling_separate_compilation_replay_ready\": "
      << (imported_input.error_handling_separate_compilation_replay_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"error_handling_deterministic\": "
      << (imported_input.error_handling_deterministic ? "true" : "false")
      << ",\n"
      << "      \"error_handling_contract_id\": \""
      << EscapeJsonString(imported_input.error_handling_contract_id) << "\",\n"
      << "      \"error_handling_source_contract_id\": \""
      << EscapeJsonString(imported_input.error_handling_source_contract_id)
      << "\",\n"
      << "      \"error_handling_result_and_bridging_artifact_replay_key\": \""
      << EscapeJsonString(
             imported_input.error_handling_result_and_bridging_artifact_replay_key)
      << "\",\n"
      << "      \"error_handling_replay_key\": \""
      << EscapeJsonString(imported_input.error_handling_error_handling_replay_key)
      << "\",\n"
      << "      \"throws_replay_key\": \""
      << EscapeJsonString(imported_input.error_handling_throws_replay_key)
      << "\",\n"
      << "      \"result_like_replay_key\": \""
      << EscapeJsonString(imported_input.error_handling_result_like_replay_key)
      << "\",\n"
      << "      \"ns_error_replay_key\": \""
      << EscapeJsonString(imported_input.error_handling_ns_error_replay_key)
      << "\",\n"
      << "      \"unwind_replay_key\": \""
      << EscapeJsonString(imported_input.error_handling_unwind_replay_key)
      << "\",\n"
      << "      \"concurrency_actor_mailbox_runtime_import_present\": "
      << (imported_input.concurrency_actor_mailbox_runtime_import_present
              ? "true"
              : "false")
      << ",\n"
      << "      \"concurrency_actor_mailbox_runtime_ready\": "
      << (imported_input.concurrency_actor_mailbox_runtime_ready ? "true"
                                                                 : "false")
      << ",\n"
      << "      \"concurrency_actor_mailbox_runtime_deterministic\": "
      << (imported_input.concurrency_actor_mailbox_runtime_deterministic
              ? "true"
              : "false")
      << ",\n"
      << "      \"concurrency_actor_contract_id\": \""
      << EscapeJsonString(imported_input.concurrency_actor_contract_id)
      << "\",\n"
      << "      \"concurrency_actor_source_contract_id\": \""
      << EscapeJsonString(imported_input.concurrency_actor_source_contract_id)
      << "\",\n"
      << "      \"concurrency_actor_mailbox_runtime_replay_key\": \""
      << EscapeJsonString(
             imported_input.concurrency_actor_mailbox_runtime_replay_key)
      << "\",\n"
      << "      \"concurrency_actor_lowering_replay_key\": \""
      << EscapeJsonString(imported_input.concurrency_actor_lowering_replay_key)
      << "\",\n"
      << "      \"concurrency_actor_isolation_lowering_replay_key\": \""
      << EscapeJsonString(
             imported_input.concurrency_actor_isolation_lowering_replay_key)
      << "\",\n"
      << "      \"interop_ffi_metadata_interface_preservation_present\": "
      << (imported_input.interop_ffi_metadata_interface_preservation_present
              ? "true"
              : "false")
      << ",\n"
      << "      \"interop_ffi_runtime_import_artifact_ready\": "
      << (imported_input.interop_ffi_runtime_import_artifact_ready ? "true"
                                                                  : "false")
      << ",\n"
      << "      \"interop_ffi_separate_compilation_preservation_ready\": "
      << (imported_input.interop_ffi_separate_compilation_preservation_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"interop_ffi_deterministic\": "
      << (imported_input.interop_ffi_deterministic ? "true" : "false")
      << ",\n"
      << "      \"interop_ffi_contract_id\": \""
      << EscapeJsonString(imported_input.interop_ffi_contract_id)
      << "\",\n"
      << "      \"interop_ffi_source_contract_id\": \""
      << EscapeJsonString(imported_input.interop_ffi_source_contract_id)
      << "\",\n"
      << "      \"interop_ffi_preservation_contract_id\": \""
      << EscapeJsonString(imported_input.interop_ffi_preservation_contract_id)
      << "\",\n"
      << "      \"interop_ffi_replay_key\": \""
      << EscapeJsonString(imported_input.interop_ffi_replay_key)
      << "\",\n"
      << "      \"interop_ffi_lowering_replay_key\": \""
      << EscapeJsonString(imported_input.interop_ffi_lowering_replay_key)
      << "\",\n"
      << "      \"interop_ffi_preservation_replay_key\": \""
      << EscapeJsonString(imported_input.interop_ffi_preservation_replay_key)
      << "\",\n"
      << "      \"interop_ffi_local_foreign_callable_count\": "
      << imported_input.interop_ffi_local_foreign_callable_count << ",\n"
      << "      \"interop_ffi_local_metadata_preservation_sites\": "
      << imported_input.interop_ffi_local_metadata_preservation_sites
      << ",\n"
      << "      \"interop_ffi_local_interface_annotation_sites\": "
      << imported_input.interop_ffi_local_interface_annotation_sites
      << ",\n"
      << "      \"interop_header_module_bridge_generation_present\": "
      << (imported_input.interop_header_module_bridge_generation_present
              ? "true"
              : "false")
      << ",\n"
      << "      \"interop_header_module_bridge_runtime_generation_ready\": "
      << (imported_input.interop_header_module_bridge_runtime_generation_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"interop_header_module_bridge_cross_module_packaging_ready\": "
      << (imported_input
                  .interop_header_module_bridge_cross_module_packaging_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"interop_header_module_bridge_deterministic\": "
      << (imported_input.interop_header_module_bridge_deterministic ? "true"
                                                                   : "false")
      << ",\n"
      << "      \"interop_header_module_bridge_contract_id\": \""
      << EscapeJsonString(
             imported_input.interop_header_module_bridge_contract_id)
      << "\",\n"
      << "      \"interop_header_module_bridge_source_contract_id\": \""
      << EscapeJsonString(
             imported_input.interop_header_module_bridge_source_contract_id)
      << "\",\n"
      << "      \"interop_header_module_bridge_preservation_contract_id\": \""
      << EscapeJsonString(
             imported_input
                 .interop_header_module_bridge_preservation_contract_id)
      << "\",\n"
      << "      \"interop_header_module_bridge_replay_key\": \""
      << EscapeJsonString(
             imported_input.interop_header_module_bridge_replay_key)
      << "\",\n"
      << "      \"interop_header_module_bridge_preservation_replay_key\": \""
      << EscapeJsonString(
             imported_input
                 .interop_header_module_bridge_preservation_replay_key)
      << "\",\n"
      << "      \"interop_bridge_header_artifact_relative_path\": \""
      << EscapeJsonString(
             imported_input.interop_bridge_header_artifact_relative_path)
      << "\",\n"
      << "      \"interop_bridge_module_artifact_relative_path\": \""
      << EscapeJsonString(
             imported_input.interop_bridge_module_artifact_relative_path)
      << "\",\n"
      << "      \"interop_bridge_artifact_relative_path\": \""
      << EscapeJsonString(imported_input.interop_bridge_artifact_relative_path)
      << "\",\n"
      << "      \"interop_header_module_bridge_local_foreign_callable_count\": "
      << imported_input.interop_header_module_bridge_local_foreign_callable_count
      << ",\n"
      << "      \"metaprogramming_macro_host_process_cache_runtime_integration_present\": "
      << (imported_input
                  .metaprogramming_macro_host_process_cache_runtime_integration_present
              ? "true"
              : "false")
      << ",\n"
      << "      \"metaprogramming_macro_host_process_cache_runtime_ready\": "
      << (imported_input.metaprogramming_macro_host_process_cache_runtime_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"metaprogramming_macro_host_process_cache_separate_compilation_ready\": "
      << (imported_input
                  .metaprogramming_macro_host_process_cache_separate_compilation_ready
              ? "true"
              : "false")
      << ",\n"
      << "      \"metaprogramming_macro_host_process_cache_deterministic\": "
      << (imported_input.metaprogramming_macro_host_process_cache_deterministic
              ? "true"
              : "false")
      << ",\n"
      << "      \"metaprogramming_macro_host_process_cache_contract_id\": \""
      << EscapeJsonString(
             imported_input.metaprogramming_macro_host_process_cache_contract_id)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_source_contract_id\": \""
      << EscapeJsonString(
             imported_input
                 .metaprogramming_macro_host_process_cache_source_contract_id)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_replay_key\": \""
      << EscapeJsonString(
             imported_input.metaprogramming_macro_host_process_cache_replay_key)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_host_executable_relative_path\": \""
      << EscapeJsonString(
             imported_input
                 .metaprogramming_macro_host_process_cache_host_executable_relative_path)
      << "\",\n"
      << "      \"metaprogramming_macro_host_process_cache_root_relative_path\": \""
      << EscapeJsonString(
             imported_input
                 .metaprogramming_macro_host_process_cache_root_relative_path)
      << "\",\n"
      << "      \"block_ownership_artifact_preservation_present\": "
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
