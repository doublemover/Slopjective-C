#pragma once

#include <filesystem>
#include <string>

#include "libobjc3c_frontend/c_api.h"

struct FrontendCApiRunnerArtifactPathView {
  std::string summary;
  std::string diagnostics;
  std::string manifest;
  std::string ir;
  std::string object;
  std::string runtime_metadata_binary;
};

FrontendCApiRunnerArtifactPathView BuildFrontendCApiRunnerArtifactPathView(
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &summary_path_text);
FrontendCApiRunnerArtifactPathView BuildFrontendCApiRunnerArtifactPathView(
    const objc3c_frontend_c_compile_result_t &result,
    const std::filesystem::path &summary_path);
