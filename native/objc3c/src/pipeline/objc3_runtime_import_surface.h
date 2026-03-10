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

bool TryLoadObjc3ImportedRuntimeModuleSurface(
    const std::filesystem::path &path,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);
