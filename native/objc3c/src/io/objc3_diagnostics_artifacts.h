#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "parse/objc3_diagnostics_bus.h"

void WriteDiagnosticsArtifacts(const std::filesystem::path &out_dir,
                               const std::string &emit_prefix,
                               const Objc3FrontendDiagnosticsBus &stage_diagnostics,
                               const std::vector<std::string> &post_pipeline_diagnostics);

void WriteDiagnosticsArtifacts(const std::filesystem::path &out_dir,
                               const std::string &emit_prefix,
                               const std::vector<std::string> &diagnostics);
