#pragma once

#include <filesystem>
#include <string>
#include <vector>

void WriteDiagnosticsArtifacts(const std::filesystem::path &out_dir,
                               const std::string &emit_prefix,
                               const std::vector<std::string> &diagnostics);
