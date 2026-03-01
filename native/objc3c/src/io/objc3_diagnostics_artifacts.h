#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "contracts/objc3_frontend_diagnostics_bus_contract.h"

void WriteDiagnosticsArtifacts(const std::filesystem::path &out_dir,
                               const std::string &emit_prefix,
                               const Objc3FrontendDiagnosticsBus &stage_diagnostics,
                               const std::vector<std::string> &post_pipeline_diagnostics);

void WriteDiagnosticsArtifacts(const std::filesystem::path &out_dir,
                               const std::string &emit_prefix,
                               const std::vector<std::string> &diagnostics);
