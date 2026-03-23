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
  std::string part6_result_and_bridging_artifact_replay_key;
  std::string part6_part6_replay_key;
  std::string part6_throws_replay_key;
  std::string part6_result_like_replay_key;
  std::string part6_ns_error_replay_key;
  std::string part6_unwind_replay_key;
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
};

bool TryLoadObjc3ImportedRuntimeModuleSurface(
    const std::filesystem::path &path,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);
bool TryLoadObjc3ImportedRuntimeModulePackagingPeerArtifacts(
    const Objc3ImportedRuntimeModuleSurface &surface,
    Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error);
