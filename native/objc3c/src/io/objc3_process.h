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
  int success_status_code = 0;
  int invalid_descriptor_status_code = 0;
  int duplicate_registration_status_code = 0;
  int out_of_order_status_code = 0;
  std::uint64_t translation_unit_registration_order_ordinal = 0;
  std::string object_artifact_relative_path;
  std::string backend_artifact_relative_path;
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

bool TryBuildObjc3RuntimeTranslationUnitRegistrationManifestArtifact(
    const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &linker_retention_artifacts,
    std::size_t runtime_metadata_binary_byte_count,
    std::string &manifest_json,
    std::string &error);
