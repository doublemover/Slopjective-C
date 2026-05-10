#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"
#include "tools/objc3c_frontend_c_api_runner_diagnostic_totals.h"

struct FrontendCApiRunnerObservabilityContext {
  FrontendCApiRunnerArtifactPathView paths;
  FrontendCApiDiagnosticTotals diagnostic_totals;
  std::string last_attempted_stage;
  std::string blocking_stage;
  std::string child_indent;
  std::string grandchild_indent;
  bool result_error_message_present = false;
};

FrontendCApiRunnerObservabilityContext
BuildFrontendCApiRunnerObservabilityContext(
    const std::string &indent,
    const std::string &summary_path_text,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text);
