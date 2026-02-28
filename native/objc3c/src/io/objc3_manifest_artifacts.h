#pragma once

#include <filesystem>
#include <string>

void WriteManifestArtifact(const std::filesystem::path &out_dir,
                           const std::string &emit_prefix,
                           const std::string &manifest_json);
