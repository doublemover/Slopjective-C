#pragma once

#include <filesystem>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

struct Objc3RuntimeMetadataLinkerRetentionArtifacts {
  std::string linker_response_file_payload;
  std::string discovery_json;
  std::string object_format;
  std::string object_artifact_relative_path;
  std::string linker_anchor_symbol;
  std::string discovery_root_symbol;
  std::string linker_anchor_logical_section;
  std::string discovery_root_logical_section;
  std::string linker_anchor_emitted_section;
  std::string discovery_root_emitted_section;
  std::string linker_response_artifact_suffix;
  std::string discovery_artifact_suffix;
  std::string translation_unit_identity_model;
  std::string translation_unit_identity_key;
  std::string driver_linker_flag;
};

struct Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs {
  std::string contract_id;
  std::string translation_unit_registration_contract_id;
  std::string runtime_support_library_link_wiring_contract_id;
  std::string manifest_payload_model;
  std::string manifest_artifact_relative_path;
  std::vector<std::string> runtime_owned_payload_artifacts;
  std::string runtime_support_library_archive_relative_path;
  std::string constructor_root_symbol;
  std::string constructor_root_ownership_model;
  std::string manifest_authority_model;
  std::string constructor_init_stub_symbol_prefix;
  std::string constructor_init_stub_ownership_model;
  std::string constructor_priority_policy;
  std::string registration_entrypoint_symbol;
  std::string translation_unit_identity_model;
  std::string launch_integration_contract_id;
  std::string runtime_library_resolution_model;
  std::string driver_linker_flag_consumption_model;
  std::string compile_wrapper_command_surface;
  std::string compile_proof_command_surface;
  std::string execution_smoke_command_surface;
  std::string dispatch_accessor_runtime_abi_contract_id;
  std::string dispatch_accessor_runtime_abi_boundary_model;
  std::string dispatch_accessor_public_header_path;
  std::string dispatch_accessor_private_header_path;
  std::string dispatch_accessor_runtime_dispatch_symbol;
  std::string dispatch_accessor_dispatch_state_snapshot_symbol;
  std::string dispatch_accessor_method_cache_state_snapshot_symbol;
  std::string dispatch_accessor_method_cache_entry_snapshot_symbol;
  std::string dispatch_accessor_property_registry_state_snapshot_symbol;
  std::string dispatch_accessor_property_entry_snapshot_symbol;
  std::string dispatch_accessor_arc_debug_state_snapshot_symbol;
  std::string dispatch_accessor_current_property_read_symbol;
  std::string dispatch_accessor_current_property_write_symbol;
  std::string dispatch_accessor_current_property_exchange_symbol;
  std::string dispatch_accessor_bind_current_property_context_symbol;
  std::string dispatch_accessor_clear_current_property_context_symbol;
  std::string dispatch_accessor_weak_current_property_load_symbol;
  std::string dispatch_accessor_weak_current_property_store_symbol;
  std::string dispatch_accessor_retain_symbol;
  std::string dispatch_accessor_release_symbol;
  std::string dispatch_accessor_autorelease_symbol;
  bool dispatch_accessor_private_testing_surface_only = false;
  bool dispatch_accessor_deterministic = false;
  std::string registration_descriptor_source_contract_id;
  std::string registration_descriptor_source_surface_path;
  std::string registration_descriptor_pragma_name;
  std::string image_root_pragma_name;
  std::string module_identity_source;
  std::string registration_descriptor_identifier;
  std::string registration_descriptor_identity_source;
  std::string image_root_identifier;
  std::string image_root_identity_source;
  std::string bootstrap_visible_metadata_ownership_model;
  std::size_t class_descriptor_count = 0;
  std::size_t protocol_descriptor_count = 0;
  std::size_t category_descriptor_count = 0;
  std::size_t property_descriptor_count = 0;
  std::size_t ivar_descriptor_count = 0;
  std::size_t total_descriptor_count = 0;
  std::string bootstrap_semantics_contract_id;
  std::string duplicate_registration_policy;
  std::string realization_order_policy;
  std::string failure_mode;
  std::string registration_result_model;
  std::string registration_order_ordinal_model;
  std::string runtime_state_snapshot_symbol;
  std::string bootstrap_runtime_api_contract_id;
  std::string bootstrap_runtime_api_public_header_path;
  std::string bootstrap_runtime_api_archive_relative_path;
  std::string bootstrap_runtime_api_registration_status_enum_type;
  std::string bootstrap_runtime_api_image_descriptor_type;
  std::string bootstrap_runtime_api_selector_handle_type;
  std::string bootstrap_runtime_api_registration_snapshot_type;
  std::string bootstrap_runtime_api_registration_entrypoint_symbol;
  std::string bootstrap_runtime_api_selector_lookup_symbol;
  std::string bootstrap_runtime_api_dispatch_entrypoint_symbol;
  std::string bootstrap_runtime_api_state_snapshot_symbol;
  std::string bootstrap_runtime_api_reset_for_testing_symbol;
  std::string bootstrap_registrar_contract_id;
  std::string bootstrap_registrar_internal_header_path;
  std::string bootstrap_registrar_stage_registration_table_symbol;
  std::string bootstrap_registrar_image_walk_snapshot_symbol;
  std::string bootstrap_registrar_image_walk_model;
  std::string bootstrap_registrar_discovery_root_validation_model;
  std::string bootstrap_registrar_selector_pool_interning_model;
  std::string bootstrap_registrar_realization_staging_model;
  std::string bootstrap_reset_contract_id;
  std::string bootstrap_reset_internal_header_path;
  std::string bootstrap_reset_replay_registered_images_symbol;
  std::string bootstrap_reset_reset_replay_state_snapshot_symbol;
  std::string bootstrap_reset_lifecycle_model;
  std::string bootstrap_reset_replay_order_model;
  std::string bootstrap_reset_image_local_init_state_reset_model;
  std::string bootstrap_reset_bootstrap_catalog_retention_model;
  std::string bootstrap_lowering_contract_id;
  std::string bootstrap_lowering_boundary_model;
  std::string bootstrap_global_ctor_list_model;
  std::string bootstrap_registration_table_layout_model;
  std::string bootstrap_image_local_initialization_model;
  std::string bootstrap_constructor_root_emission_state;
  std::string bootstrap_init_stub_emission_state;
  std::string bootstrap_registration_table_emission_state;
  std::string bootstrap_registration_table_symbol_prefix;
  std::string bootstrap_image_local_init_state_symbol_prefix;
  std::uint64_t bootstrap_registration_table_abi_version = 0;
  std::uint64_t bootstrap_registration_table_pointer_field_count = 0;
  int success_status_code = 0;
  int invalid_descriptor_status_code = 0;
  int duplicate_registration_status_code = 0;
  int out_of_order_status_code = 0;
  std::uint64_t translation_unit_registration_order_ordinal = 0;
  std::string object_artifact_relative_path;
  std::string backend_artifact_relative_path;
};

struct Objc3RuntimeRegistrationDescriptorArtifactInputs {
  std::string contract_id;
  std::string registration_manifest_contract_id;
  std::string source_surface_contract_id;
  std::string payload_model;
  std::string artifact_relative_path;
  std::string authority_model;
  std::string translation_unit_identity_model;
  std::string payload_ownership_model;
  std::string runtime_support_library_archive_relative_path;
  std::string registration_entrypoint_symbol;
  std::string registration_descriptor_pragma_name;
  std::string image_root_pragma_name;
  std::string module_identity_source;
  std::string registration_descriptor_identifier;
  std::string registration_descriptor_identity_source;
  std::string image_root_identifier;
  std::string image_root_identity_source;
  std::string bootstrap_visible_metadata_ownership_model;
  std::string constructor_root_symbol;
  std::string constructor_init_stub_symbol_prefix;
  std::string bootstrap_registration_table_symbol_prefix;
  std::string bootstrap_image_local_init_state_symbol_prefix;
  std::size_t class_descriptor_count = 0;
  std::size_t protocol_descriptor_count = 0;
  std::size_t category_descriptor_count = 0;
  std::size_t property_descriptor_count = 0;
  std::size_t ivar_descriptor_count = 0;
  std::size_t total_descriptor_count = 0;
  std::uint64_t translation_unit_registration_order_ordinal = 0;
  std::string object_artifact_relative_path;
  std::string backend_artifact_relative_path;
};

struct Objc3CrossModuleRuntimeLinkPlanImportedInput {
  std::string module_name;
  std::string import_surface_artifact_path;
  std::string registration_manifest_artifact_path;
  std::string object_artifact_path;
  std::string discovery_artifact_path;
  std::string linker_response_artifact_path;
  std::string translation_unit_identity_model;
  std::string translation_unit_identity_key;
  std::string object_format;
  std::string runtime_support_library_archive_relative_path;
  std::uint64_t translation_unit_registration_order_ordinal = 0;
  std::vector<std::string> driver_linker_flags;
  bool part6_result_and_bridging_artifact_replay_present = false;
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
  bool part7_actor_mailbox_runtime_import_present = false;
  bool part7_actor_mailbox_runtime_ready = false;
  bool part7_actor_mailbox_runtime_deterministic = false;
  std::string part7_actor_contract_id;
  std::string part7_actor_source_contract_id;
  std::string part7_actor_mailbox_runtime_replay_key;
  std::string part7_actor_lowering_replay_key;
  std::string part7_actor_isolation_lowering_replay_key;
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
  bool part10_macro_host_process_cache_runtime_integration_present = false;
  bool part10_macro_host_process_cache_runtime_ready = false;
  bool part10_macro_host_process_cache_separate_compilation_ready = false;
  bool part10_macro_host_process_cache_deterministic = false;
  std::string part10_macro_host_process_cache_contract_id;
  std::string part10_macro_host_process_cache_source_contract_id;
  std::string part10_macro_host_process_cache_replay_key;
  std::string part10_macro_host_process_cache_host_executable_relative_path;
  std::string part10_macro_host_process_cache_root_relative_path;
};

struct Objc3CrossModuleRuntimeLinkPlanArtifactInputs {
  std::string contract_id;
  std::string source_orchestration_contract_id;
  std::string import_surface_contract_id;
  std::string registration_manifest_contract_id;
  std::string payload_model;
  std::string artifact_relative_path;
  std::string linker_response_artifact_relative_path;
  std::string authority_model;
  std::string packaging_model;
  std::string registration_scope_model;
  std::string link_object_order_model;
  std::string local_module_name;
  std::string local_import_surface_artifact_relative_path;
  std::string local_registration_manifest_artifact_relative_path;
  std::string local_object_artifact_relative_path;
  std::string runtime_support_library_archive_relative_path;
  std::string object_format;
  std::string local_translation_unit_identity_model;
  std::string local_translation_unit_identity_key;
  std::uint64_t local_translation_unit_registration_order_ordinal = 0;
  std::vector<std::string> local_driver_linker_flags;
  std::string expected_part6_contract_id;
  std::string expected_part6_source_contract_id;
  std::string expected_part7_actor_contract_id;
  std::string expected_part7_actor_source_contract_id;
  std::string expected_part11_ffi_contract_id;
  std::string expected_part11_ffi_source_contract_id;
  std::string expected_part11_ffi_preservation_contract_id;
  std::string expected_part11_header_module_bridge_contract_id;
  std::string expected_part11_header_module_bridge_source_contract_id;
  std::string expected_part11_header_module_bridge_preservation_contract_id;
  std::string expected_part11_bridge_header_artifact_relative_path;
  std::string expected_part11_bridge_module_artifact_relative_path;
  std::string expected_part11_bridge_artifact_relative_path;
  std::string expected_part10_host_cache_contract_id;
  std::string expected_part10_host_cache_source_contract_id;
  std::string expected_part10_host_cache_executable_relative_path;
  std::string expected_part10_host_cache_root_relative_path;
  std::vector<std::string> direct_import_surface_artifact_paths;
  std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput> imported_inputs;
};

struct Objc3Part10MacroHostProcessCacheArtifactInputs {
  std::string contract_id;
  std::string source_contract_id;
  std::string surface_path;
  std::string artifact_relative_path;
  std::string host_executable_relative_path;
  std::string cache_root_relative_path;
  std::string host_model;
  std::string toolchain_model;
  std::string cache_model;
  std::string fail_closed_model;
  std::string replay_key;
  bool deterministic = false;
};

struct Objc3ConformanceReportPublicationArtifactInputs {
  std::string contract_id;
  std::string schema_id;
  std::string selected_profile;
  bool selected_profile_supported = false;
  std::vector<std::string> supported_profile_ids;
  std::vector<std::string> rejected_profile_ids;
  std::string effective_compatibility_mode;
  bool migration_assist_enabled = false;
  std::string publication_model;
  std::string publication_surface_kind;
  std::string fail_closed_diagnostic_model;
  std::string lowered_report_contract_id;
  std::string runtime_capability_contract_id;
  std::string public_conformance_schema_id;
  std::string advanced_feature_ops_contract_id;
  std::string advanced_feature_reporting_contract_id;
  std::string advanced_feature_release_evidence_contract_id;
  std::string ci_release_evidence_gate_script_path;
  std::string runbook_reference_path;
  std::string dashboard_schema_path;
  std::vector<std::string> advanced_feature_targeted_profile_ids;
  std::string report_artifact_relative_path;
};

struct Objc3ConformanceClaimValidationArtifactInputs {
  std::string report_artifact_path;
  std::string publication_artifact_path;
};

struct Objc3ReleaseEvidenceOperationArtifactInputs {
  std::string report_artifact_path;
  std::string publication_artifact_path;
  std::string validation_artifact_path;
  std::string dashboard_artifact_path;
};

struct Objc3DashboardStatusArtifactInputs {
  std::string report_artifact_path;
  std::string publication_artifact_path;
  std::string validation_artifact_path;
  std::string release_evidence_operation_artifact_path;
};

struct Objc3AdvancedFeatureGateArtifactInputs {
  std::string surface_kind;
  std::string report_artifact_path;
  std::string publication_artifact_path;
  std::string validation_artifact_path;
  std::string release_evidence_operation_artifact_path;
  std::string dashboard_artifact_path;
};

struct Objc3ReleaseCandidateMatrixArtifactInputs {
  std::string surface_kind;
  std::string report_artifact_path;
  std::string publication_artifact_path;
  std::string advanced_feature_gate_artifact_path;
  std::string validation_artifact_path;
  std::string release_evidence_operation_artifact_path;
  std::string dashboard_artifact_path;
};

int RunProcess(const std::string &executable, const std::vector<std::string> &args);

int RunObjectiveCCompile(const std::filesystem::path &clang_path,
                         const std::filesystem::path &input,
                         const std::filesystem::path &object_out);

int RunIRCompile(const std::filesystem::path &clang_path,
                 const std::filesystem::path &ir_path,
                 const std::filesystem::path &object_out);

int RunIRCompileLLVMDirect(const std::filesystem::path &llc_path,
                           const std::filesystem::path &ir_path,
                           const std::filesystem::path &object_out,
                           std::string &error);

bool TryBuildObjc3RuntimeMetadataLinkerRetentionArtifacts(
    const std::filesystem::path &ir_path,
    const std::filesystem::path &object_out,
    Objc3RuntimeMetadataLinkerRetentionArtifacts &artifacts,
    std::string &error);
bool TryBuildObjc3ReleaseEvidenceOperationArtifact(
    const Objc3ReleaseEvidenceOperationArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &validation_json,
    std::string &artifact_json,
    std::string &error);
bool TryBuildObjc3DashboardStatusArtifact(
    const Objc3DashboardStatusArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &validation_json,
    const std::string &release_evidence_operation_json,
    std::string &artifact_json,
    std::string &error);
bool TryBuildObjc3AdvancedFeatureGateArtifact(
    const Objc3AdvancedFeatureGateArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    std::string &artifact_json,
    std::string &error);
bool TryBuildObjc3ReleaseCandidateMatrixArtifact(
    const Objc3ReleaseCandidateMatrixArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &advanced_feature_gate_json,
    std::string &artifact_json,
    std::string &error);

bool TryBuildObjc3RuntimeTranslationUnitRegistrationManifestArtifact(
    const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &linker_retention_artifacts,
    std::size_t runtime_metadata_binary_byte_count,
    std::string &manifest_json,
    std::string &error);
bool TryBuildObjc3RuntimeRegistrationDescriptorArtifact(
    const Objc3RuntimeRegistrationDescriptorArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &linker_retention_artifacts,
    std::string &descriptor_json,
    std::string &error);
bool TryBuildObjc3CrossModuleRuntimeLinkPlanArtifact(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    std::string &plan_json,
    std::string &linker_response_payload,
    std::string &error);
bool TryBuildObjc3Part10MacroHostProcessCacheArtifact(
    const Objc3Part10MacroHostProcessCacheArtifactInputs &inputs,
    const std::filesystem::path &source_input_path,
    std::string &artifact_json,
    std::string &error);
bool TryBuildObjc3ConformanceReportPublicationArtifact(
    const Objc3ConformanceReportPublicationArtifactInputs &inputs,
    std::string &artifact_json,
    std::string &error);
bool TryBuildObjc3ConformanceClaimValidationArtifact(
    const Objc3ConformanceClaimValidationArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    std::string &artifact_json,
    std::string &error);
