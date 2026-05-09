#pragma once

/*
 * Internal C++ artifact path planning owner. Public callers observe only the
 * result-owned paths selected through objc3c_frontend_artifact.h.
 */
#include <filesystem>
#include <string>

#include "libobjc3c_frontend/objc3c_frontend_options.h"

namespace objc3c::frontend {

struct Objc3FrontendArtifactOutputPlan {
  std::filesystem::path input_path;
  std::filesystem::path out_dir;
  std::string emit_prefix;
  bool has_out_dir = false;
  bool wants_ir_file = false;
  bool wants_emit_stage = false;
};

Objc3FrontendArtifactOutputPlan BuildFrontendArtifactOutputPlan(
    const objc3c_frontend_compile_options_t &options,
    const std::filesystem::path &input_path);

std::filesystem::path BuildFrontendDiagnosticsOutputPath(
    const Objc3FrontendArtifactOutputPlan &plan);

std::filesystem::path BuildFrontendManifestOutputPath(
    const Objc3FrontendArtifactOutputPlan &plan);

std::filesystem::path BuildFrontendIrOutputPath(
    const Objc3FrontendArtifactOutputPlan &plan);

std::filesystem::path BuildFrontendObjectOutputPath(
    const Objc3FrontendArtifactOutputPlan &plan);

std::filesystem::path BuildFrontendObjectBackendOutputPath(
    const Objc3FrontendArtifactOutputPlan &plan);

}  // namespace objc3c::frontend
