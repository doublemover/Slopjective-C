#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_artifact_paths.h"
#include "tools/objc3c_frontend_c_api_runner_diagnostic_totals.h"

struct FrontendCApiRunnerDumpPublication;
struct FrontendCApiRunnerPublicResultView;

struct FrontendCApiRunnerObservabilityDiagnosticSnapshot {
  FrontendCApiDiagnosticTotals totals;
  std::string result_error_message;
  bool result_error_message_present = false;
};

struct FrontendCApiRunnerObservabilityPublication {
  objc3c_frontend_c_status_t status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
  FrontendCApiRunnerArtifactPathView paths;
  FrontendCApiRunnerObservabilityDiagnosticSnapshot diagnostics;
  std::string last_attempted_stage;
  std::string blocking_stage;
};

FrontendCApiRunnerObservabilityPublication
BuildFrontendCApiRunnerObservabilityPublication(
    const FrontendCApiRunnerDumpPublication &publication);

FrontendCApiRunnerObservabilityPublication
BuildFrontendCApiRunnerObservabilityPublication(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const FrontendCApiRunnerPublicResultView &public_result);
