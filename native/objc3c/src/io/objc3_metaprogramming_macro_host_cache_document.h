#pragma once

#include <filesystem>
#include <string>

#include "io/objc3_process.h"

std::string BuildObjc3MetaprogrammingMacroHostProcessCacheArtifactDocumentJson(
    const Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs &inputs,
    const std::filesystem::path &source_input_path,
    const std::string &cache_key,
    const std::filesystem::path &cache_entry,
    const std::filesystem::path &cache_summary_path,
    const std::filesystem::path &cache_runtime_import_path,
    const std::filesystem::path &cache_manifest_path,
    const std::string &cache_key_material_digest,
    const std::string &cache_validation_state,
    bool launch_attempted,
    bool cache_hit,
    int host_process_exit_code);
