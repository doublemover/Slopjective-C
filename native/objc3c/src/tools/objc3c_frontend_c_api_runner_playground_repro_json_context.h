#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"

struct FrontendCApiRunnerPlaygroundReproContext {
  FrontendCApiRunnerArtifactPathView paths;
  std::string child_indent;
  std::string grandchild_indent;
};

FrontendCApiRunnerPlaygroundReproContext
BuildFrontendCApiRunnerPlaygroundReproContext(
    const std::string &indent,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &summary_path_text);
