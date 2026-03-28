#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "pipeline/objc3_frontend_types.h"

struct Objc3ImportedRuntimeModuleSurface {
  std::filesystem::path source_path;
  Objc3RuntimeAwareImportModuleFrontendClosureSummary frontend_closure_summary;
  Objc3RuntimeMetadataSourceRecordSet runtime_metadata_source_records;
  std::vector<std::string> reused_module_names_lexicographic;
  bool uses_serialized_runtime_metadata_payload = false;
  bool type_system_optional_keypath_lowering_contract_present = false;
  bool error_handling_result_and_bridging_artifact_replay_present = false;
  bool concurrency_actor_mailbox_runtime_import_present = false;
  bool interop_foreign_surface_interface_preservation_present = false;
  bool metaprogramming_module_interface_replay_preservation_present = false;
  bool metaprogramming_macro_host_process_cache_runtime_integration_present = false;
  bool dispatch_dispatch_metadata_interface_preservation_present = false;
  bool block_ownership_artifact_preservation_present = false;
  bool storage_reflection_artifact_preservation_present = false;
  std::size_t type_system_optional_send_sites = 0;
  std::size_t type_system_typed_keypath_literal_sites = 0;
  std::size_t type_system_live_optional_lowering_sites = 0;
  std::size_t type_system_live_typed_keypath_artifact_sites = 0;
  bool type_system_ready_for_native_optional_lowering = false;
  bool type_system_optional_send_runtime_ready = false;
  bool type_system_typed_keypath_descriptor_handles_ready = false;
  bool type_system_typed_keypath_runtime_execution_helper_landed = false;
  std::string type_system_optional_keypath_lowering_replay_key;
  std::string type_system_optional_keypath_runtime_helper_replay_key;
  bool error_handling_binary_artifact_replay_ready = false;
  bool error_handling_runtime_import_artifact_ready = false;
  bool error_handling_separate_compilation_replay_ready = false;
  bool error_handling_deterministic = false;
  std::string error_handling_contract_id;
  std::string error_handling_source_contract_id;
  std::string error_handling_result_and_bridging_artifact_replay_key;
  std::string error_handling_error_handling_replay_key;
  std::string error_handling_throws_replay_key;
  std::string error_handling_result_like_replay_key;
  std::string error_handling_ns_error_replay_key;
  std::string error_handling_unwind_replay_key;
  bool concurrency_actor_mailbox_runtime_ready = false;
  bool concurrency_actor_mailbox_runtime_deterministic = false;
  std::string concurrency_actor_mailbox_runtime_contract_id;
  std::string concurrency_actor_mailbox_runtime_source_contract_id;
  std::string concurrency_actor_mailbox_runtime_replay_key;
  std::string concurrency_actor_lowering_replay_key;
  std::string concurrency_actor_isolation_lowering_replay_key;
  bool interop_runtime_import_artifact_ready = false;
  bool interop_separate_compilation_preservation_ready = false;
  bool interop_deterministic = false;
  std::string interop_contract_id;
  std::string interop_foreign_import_source_contract_id;
  std::string interop_cpp_swift_source_contract_id;
  std::string interop_replay_key;
  std::string interop_foreign_import_source_replay_key;
  std::string interop_cpp_swift_source_replay_key;
  std::size_t interop_local_foreign_callable_count = 0;
  std::size_t interop_local_import_module_annotation_count = 0;
  std::size_t interop_local_imported_module_name_count = 0;
  std::size_t interop_local_swift_name_annotation_count = 0;
  std::size_t interop_local_swift_private_annotation_count = 0;
  std::size_t interop_local_cpp_name_annotation_count = 0;
  std::size_t interop_local_header_name_annotation_count = 0;
  std::size_t interop_local_named_annotation_payload_count = 0;
  bool interop_ffi_metadata_interface_preservation_present = false;
  bool interop_ffi_runtime_import_artifact_ready = false;
  bool interop_ffi_separate_compilation_preservation_ready = false;
  bool interop_ffi_deterministic = false;
  std::string interop_ffi_contract_id;
  std::string interop_ffi_source_contract_id;
  std::string interop_ffi_preservation_contract_id;
  std::string interop_ffi_replay_key;
  std::string interop_ffi_lowering_replay_key;
  std::string interop_ffi_preservation_replay_key;
  std::size_t interop_ffi_local_foreign_callable_count = 0;
  std::size_t interop_ffi_local_metadata_preservation_sites = 0;
  std::size_t interop_ffi_local_interface_annotation_sites = 0;
  bool interop_header_module_bridge_generation_present = false;
  bool interop_header_module_bridge_runtime_generation_ready = false;
  bool interop_header_module_bridge_cross_module_packaging_ready = false;
  bool interop_header_module_bridge_deterministic = false;
  std::string interop_header_module_bridge_contract_id;
  std::string interop_header_module_bridge_source_contract_id;
  std::string interop_header_module_bridge_preservation_contract_id;
  std::string interop_header_module_bridge_replay_key;
  std::string interop_header_module_bridge_preservation_replay_key;
  std::string interop_bridge_header_artifact_relative_path;
  std::string interop_bridge_module_artifact_relative_path;
  std::string interop_bridge_artifact_relative_path;
  std::size_t interop_header_module_bridge_local_foreign_callable_count = 0;
  bool metaprogramming_runtime_import_artifact_ready = false;
  bool metaprogramming_separate_compilation_preservation_ready = false;
  bool metaprogramming_deterministic = false;
  std::string metaprogramming_contract_id;
  std::string metaprogramming_source_contract_id;
  std::string metaprogramming_replay_key;
  std::string metaprogramming_expansion_lowering_replay_key;
  std::string metaprogramming_synthesized_emission_replay_key;
  bool metaprogramming_macro_host_process_cache_runtime_ready = false;
  bool metaprogramming_macro_host_process_cache_separate_compilation_ready = false;
  bool metaprogramming_macro_host_process_cache_deterministic = false;
  std::string metaprogramming_macro_host_process_cache_contract_id;
  std::string metaprogramming_macro_host_process_cache_source_contract_id;
  std::string metaprogramming_macro_host_process_cache_replay_key;
  std::string metaprogramming_macro_host_process_cache_host_executable_relative_path;
  std::string metaprogramming_macro_host_process_cache_root_relative_path;
  std::size_t metaprogramming_local_derive_method_count = 0;
  std::size_t metaprogramming_local_macro_artifact_count = 0;
  std::size_t metaprogramming_local_interface_property_behavior_artifact_count = 0;
  std::size_t metaprogramming_local_implementation_property_behavior_artifact_count = 0;
  std::size_t metaprogramming_local_runtime_method_list_count = 0;
  bool dispatch_runtime_import_artifact_ready = false;
  bool dispatch_separate_compilation_preservation_ready = false;
  bool dispatch_deterministic = false;
  std::string dispatch_contract_id;
  std::string dispatch_source_contract_id;
  std::string dispatch_replay_key;
  std::string dispatch_lowering_replay_key;
  std::size_t dispatch_local_direct_callable_record_count = 0;
  std::size_t dispatch_local_final_callable_record_count = 0;
  std::size_t dispatch_local_final_container_record_count = 0;
  std::size_t dispatch_local_sealed_container_record_count = 0;
  bool block_ownership_runtime_import_artifact_ready = false;
  bool block_ownership_separate_compilation_preservation_ready = false;
  bool block_ownership_runtime_support_library_link_wiring_ready = false;
  bool block_ownership_deterministic = false;
  std::string block_ownership_contract_id;
  std::string block_ownership_source_contract_id;
  std::string block_ownership_object_invoke_thunk_lowering_contract_id;
  std::string block_ownership_byref_helper_lowering_contract_id;
  std::string block_ownership_escape_runtime_hook_lowering_contract_id;
  std::string block_ownership_runtime_support_library_link_wiring_contract_id;
  std::string block_ownership_replay_key;
  std::size_t block_ownership_local_block_literal_sites = 0;
  std::size_t block_ownership_local_invoke_trampoline_symbolized_sites = 0;
  std::size_t block_ownership_local_copy_helper_required_sites = 0;
  std::size_t block_ownership_local_dispose_helper_required_sites = 0;
  std::size_t block_ownership_local_copy_helper_symbolized_sites = 0;
  std::size_t block_ownership_local_dispose_helper_symbolized_sites = 0;
  std::size_t block_ownership_local_escape_to_heap_sites = 0;
  std::size_t block_ownership_local_byref_layout_symbolized_sites = 0;
  bool storage_reflection_runtime_import_artifact_ready = false;
  bool storage_reflection_separate_compilation_preservation_ready = false;
  bool storage_reflection_deterministic = false;
  std::string storage_reflection_contract_id;
  std::string storage_reflection_source_contract_id;
  std::string
      storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id;
  std::string
      storage_reflection_executable_property_accessor_layout_lowering_contract_id;
  std::string storage_reflection_executable_ivar_layout_emission_contract_id;
  std::string
      storage_reflection_executable_synthesized_accessor_property_lowering_contract_id;
  std::string storage_reflection_replay_key;
  std::size_t storage_reflection_local_property_descriptor_count = 0;
  std::size_t storage_reflection_local_ivar_descriptor_count = 0;
  std::size_t storage_reflection_implementation_owned_property_entries = 0;
  std::size_t storage_reflection_synthesized_accessor_owner_entries = 0;
  std::size_t storage_reflection_synthesized_getter_entries = 0;
  std::size_t storage_reflection_synthesized_setter_entries = 0;
  std::size_t storage_reflection_synthesized_accessor_entries = 0;
  std::size_t storage_reflection_current_property_read_entries = 0;
  std::size_t storage_reflection_current_property_write_entries = 0;
  std::size_t storage_reflection_current_property_exchange_entries = 0;
  std::size_t storage_reflection_weak_current_property_load_entries = 0;
  std::size_t storage_reflection_weak_current_property_store_entries = 0;
  std::size_t storage_reflection_ivar_layout_entries = 0;
  std::size_t storage_reflection_ivar_layout_owner_entries = 0;
};

struct Objc3ImportedRuntimeModulePackagingPeerArtifacts {
  std::filesystem::path registration_manifest_path;
  std::filesystem::path object_artifact_path;
  std::filesystem::path discovery_artifact_path;
  std::filesystem::path linker_response_artifact_path;
  std::string runtime_support_library_archive_relative_path;
  std::string translation_unit_identity_model;
  std::string translation_unit_identity_key;
  std::string object_format;
  std::uint64_t translation_unit_registration_order_ordinal = 0;
  std::size_t class_descriptor_count = 0;
  std::size_t protocol_descriptor_count = 0;
  std::size_t category_descriptor_count = 0;
  std::size_t property_descriptor_count = 0;
  std::size_t ivar_descriptor_count = 0;
  std::size_t total_descriptor_count = 0;
  std::vector<std::string> driver_linker_flags;
  bool ready_for_live_registration_discovery_replay = false;
  bool ready_for_live_restart_hardening = false;
  std::string bootstrap_live_registration_contract_id;
  std::string bootstrap_live_restart_hardening_contract_id;
  std::string bootstrap_live_replay_registered_images_symbol;
  std::string bootstrap_live_reset_replay_state_snapshot_symbol;
  std::string bootstrap_live_restart_reset_for_testing_symbol;
  std::string bootstrap_live_restart_replay_registered_images_symbol;
  std::string bootstrap_live_restart_reset_replay_state_snapshot_symbol;
};

bool TryLoadObjc3ImportedRuntimeModuleSurface(
    const std::filesystem::path &path,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);
bool TryLoadObjc3ImportedRuntimeModulePackagingPeerArtifacts(
    const Objc3ImportedRuntimeModuleSurface &surface,
    Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error);
