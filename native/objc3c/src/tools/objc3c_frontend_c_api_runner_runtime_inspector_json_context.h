#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"

struct FrontendCApiRunnerRuntimeInspectorContext {
  FrontendCApiRunnerArtifactPathView paths;
  std::string child_indent;
  std::string grandchild_indent;
  bool available = false;
  std::string availability_reason;
};

FrontendCApiRunnerRuntimeInspectorContext
BuildFrontendCApiRunnerRuntimeInspectorContext(
    const std::string &indent,
    const objc3c_frontend_c_compile_result_t &result);
