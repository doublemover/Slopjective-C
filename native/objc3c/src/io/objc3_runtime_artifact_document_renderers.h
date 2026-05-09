#pragma once

#include <filesystem>
#include <string>

#include "io/objc3_process.h"

std::string BuildObjc3RuntimeMetadataLinkerRetentionDiscoveryJson(
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &artifacts);

std::string BuildObjc3RuntimeRegistrationDescriptorArtifactDocumentJson(
    const Objc3RuntimeRegistrationDescriptorArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &linker_retention_artifacts,
    const std::string &constructor_init_stub_symbol,
    const std::string &bootstrap_registration_table_symbol,
    const std::string &bootstrap_image_local_init_state_symbol);

std::string BuildObjc3MetaprogrammingMacroHostProcessCacheArtifactDocumentJson(
    const Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs &inputs,
    const std::filesystem::path &source_input_path,
    const std::string &cache_key,
    const std::filesystem::path &cache_entry,
    const std::filesystem::path &cache_summary_path,
    const std::filesystem::path &cache_runtime_import_path,
    const std::filesystem::path &cache_manifest_path,
    bool launch_attempted,
    bool cache_hit,
    int host_process_exit_code);
