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
  bool part3_optional_keypath_lowering_contract_present = false;
  bool part6_result_and_bridging_artifact_replay_present = false;
  bool part7_actor_mailbox_runtime_import_present = false;
  bool part11_foreign_surface_interface_preservation_present = false;
  bool part10_module_interface_replay_preservation_present = false;
  bool part10_macro_host_process_cache_runtime_integration_present = false;
  bool part9_dispatch_metadata_interface_preservation_present = false;
  std::size_t part3_optional_send_sites = 0;
  std::size_t part3_typed_keypath_literal_sites = 0;
  std::size_t part3_live_optional_lowering_sites = 0;
  std::size_t part3_live_typed_keypath_artifact_sites = 0;
  bool part3_ready_for_native_optional_lowering = false;
  bool part3_optional_send_runtime_ready = false;
  bool part3_typed_keypath_descriptor_handles_ready = false;
  bool part3_typed_keypath_runtime_execution_helper_landed = false;
  std::string part3_optional_keypath_lowering_replay_key;
  std::string part3_optional_keypath_runtime_helper_replay_key;
  bool part6_binary_artifact_replay_ready = false;
  bool part6_runtime_import_artifact_ready = false;
  bool part6_separate_compilation_replay_ready = false;
  bool part6_deterministic = false;
  std::string part6_contract_id;
  std::string part6_source_contract_id;
  std::string part6_result_and_bridging_artifact_replay_key;
  std::string part6_part6_replay_key;
  std::string part6_throws_replay_key;
  std::string part6_result_like_replay_key;
  std::string part6_ns_error_replay_key;
  std::string part6_unwind_replay_key;
  bool part7_actor_mailbox_runtime_ready = false;
  bool part7_actor_mailbox_runtime_deterministic = false;
  std::string part7_actor_mailbox_runtime_contract_id;
  std::string part7_actor_mailbox_runtime_source_contract_id;
  std::string part7_actor_mailbox_runtime_replay_key;
  std::string part7_actor_lowering_replay_key;
  std::string part7_actor_isolation_lowering_replay_key;
  bool part11_runtime_import_artifact_ready = false;
  bool part11_separate_compilation_preservation_ready = false;
  bool part11_deterministic = false;
  std::string part11_contract_id;
  std::string part11_foreign_import_source_contract_id;
  std::string part11_cpp_swift_source_contract_id;
  std::string part11_replay_key;
  std::string part11_foreign_import_source_replay_key;
  std::string part11_cpp_swift_source_replay_key;
  std::size_t part11_local_foreign_callable_count = 0;
  std::size_t part11_local_import_module_annotation_count = 0;
  std::size_t part11_local_imported_module_name_count = 0;
  std::size_t part11_local_swift_name_annotation_count = 0;
  std::size_t part11_local_swift_private_annotation_count = 0;
  std::size_t part11_local_cpp_name_annotation_count = 0;
  std::size_t part11_local_header_name_annotation_count = 0;
  std::size_t part11_local_named_annotation_payload_count = 0;
  bool part11_ffi_metadata_interface_preservation_present = false;
  bool part11_ffi_runtime_import_artifact_ready = false;
  bool part11_ffi_separate_compilation_preservation_ready = false;
  bool part11_ffi_deterministic = false;
  std::string part11_ffi_contract_id;
  std::string part11_ffi_source_contract_id;
  std::string part11_ffi_preservation_contract_id;
  std::string part11_ffi_replay_key;
  std::string part11_ffi_lowering_replay_key;
  std::string part11_ffi_preservation_replay_key;
  std::size_t part11_ffi_local_foreign_callable_count = 0;
  std::size_t part11_ffi_local_metadata_preservation_sites = 0;
  std::size_t part11_ffi_local_interface_annotation_sites = 0;
  bool part11_header_module_bridge_generation_present = false;
  bool part11_header_module_bridge_runtime_generation_ready = false;
  bool part11_header_module_bridge_cross_module_packaging_ready = false;
  bool part11_header_module_bridge_deterministic = false;
  std::string part11_header_module_bridge_contract_id;
  std::string part11_header_module_bridge_source_contract_id;
  std::string part11_header_module_bridge_preservation_contract_id;
  std::string part11_header_module_bridge_replay_key;
  std::string part11_header_module_bridge_preservation_replay_key;
  std::string part11_bridge_header_artifact_relative_path;
  std::string part11_bridge_module_artifact_relative_path;
  std::string part11_bridge_artifact_relative_path;
  std::size_t part11_header_module_bridge_local_foreign_callable_count = 0;
  bool part10_runtime_import_artifact_ready = false;
  bool part10_separate_compilation_preservation_ready = false;
  bool part10_deterministic = false;
  std::string part10_contract_id;
  std::string part10_source_contract_id;
  std::string part10_replay_key;
  std::string part10_expansion_lowering_replay_key;
  std::string part10_synthesized_emission_replay_key;
  bool part10_macro_host_process_cache_runtime_ready = false;
  bool part10_macro_host_process_cache_separate_compilation_ready = false;
  bool part10_macro_host_process_cache_deterministic = false;
  std::string part10_macro_host_process_cache_contract_id;
  std::string part10_macro_host_process_cache_source_contract_id;
  std::string part10_macro_host_process_cache_replay_key;
  std::string part10_macro_host_process_cache_host_executable_relative_path;
  std::string part10_macro_host_process_cache_root_relative_path;
  std::size_t part10_local_derive_method_count = 0;
  std::size_t part10_local_macro_artifact_count = 0;
  std::size_t part10_local_interface_property_behavior_artifact_count = 0;
  std::size_t part10_local_implementation_property_behavior_artifact_count = 0;
  std::size_t part10_local_runtime_method_list_count = 0;
  bool part9_runtime_import_artifact_ready = false;
  bool part9_separate_compilation_preservation_ready = false;
  bool part9_deterministic = false;
  std::string part9_contract_id;
  std::string part9_source_contract_id;
  std::string part9_replay_key;
  std::string part9_lowering_replay_key;
  std::size_t part9_local_direct_callable_record_count = 0;
  std::size_t part9_local_final_callable_record_count = 0;
  std::size_t part9_local_final_container_record_count = 0;
  std::size_t part9_local_sealed_container_record_count = 0;
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
