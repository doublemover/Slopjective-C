#pragma once

#include <cstdint>
#include <string>

#include "libobjc3c_frontend/c_api.h"

struct FrontendCApiDiagnosticTotals {
  std::uint64_t total = 0;
  std::uint64_t notes = 0;
  std::uint64_t warnings = 0;
  std::uint64_t errors = 0;
  std::uint64_t fatals = 0;
};

struct FrontendCApiCompileResultGuard {
  objc3c_frontend_c_compile_result_t *result = nullptr;

  ~FrontendCApiCompileResultGuard();
};

std::string OptionalFrontendCApiString(
    const objc3c_frontend_c_string_t *value);
std::string FrontendCApiResultArtifactPath(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind);
std::string FrontendCApiResultErrorMessage(
    const objc3c_frontend_c_compile_result_t &result);
bool FrontendCApiStageSummaryShapeReady(
    const objc3c_frontend_c_stage_summary_t &summary,
    objc3c_frontend_c_stage_id_t expected_stage);
bool FrontendCApiStageReportShapeReady(
    const objc3c_frontend_c_compile_result_t &result);
bool ValidateFrontendCApiResultAccessors(
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const std::string &result_error_message,
    std::string &reason);
FrontendCApiDiagnosticTotals BuildFrontendCApiDiagnosticTotals(
    const objc3c_frontend_c_compile_result_t &result);
const char *FrontendCApiStatusName(objc3c_frontend_c_status_t status);
const char *HighestFrontendCApiDiagnosticSeverity(
    const FrontendCApiDiagnosticTotals &totals);
std::string LastAttemptedFrontendCApiStageName(
    const objc3c_frontend_c_compile_result_t &result);
std::string BlockingFrontendCApiStageName(
    const objc3c_frontend_c_compile_result_t &result);
int FrontendCApiExitCodeFromStatus(
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result);
std::string ReadFrontendCApiLastError(
    const objc3c_frontend_c_context_t *context);
