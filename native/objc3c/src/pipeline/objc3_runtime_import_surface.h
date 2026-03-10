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
