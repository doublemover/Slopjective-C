#include "tools/objc3c_frontend_c_api_runner_result.h"

#include <cstddef>

FrontendCApiCompileResultGuard::~FrontendCApiCompileResultGuard() {
  objc3c_frontend_c_result_destroy(result);
}

std::string OptionalFrontendCApiString(
    const objc3c_frontend_c_string_t *value) {
  const objc3c_frontend_c_string_view_t view =
      objc3c_frontend_c_string_view(value);
  if (view.data == nullptr || view.size == 0) {
    return "";
  }
  return std::string(view.data, view.size);
}

std::string FrontendCApiResultArtifactPath(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return OptionalFrontendCApiString(
      objc3c_frontend_c_result_artifact_path(&result, artifact_kind));
}

std::string FrontendCApiResultErrorMessage(
    const objc3c_frontend_c_compile_result_t &result) {
  return OptionalFrontendCApiString(
      objc3c_frontend_c_result_error_message(&result));
}

bool FrontendCApiStageSummaryShapeReady(
    const objc3c_frontend_c_stage_summary_t &summary,
    objc3c_frontend_c_stage_id_t expected_stage) {
  return objc3c_frontend_c_stage_summary_is_well_formed(&summary,
                                                        expected_stage) != 0u;
}

bool FrontendCApiStageReportShapeReady(
    const objc3c_frontend_c_compile_result_t &result) {
  return FrontendCApiStageSummaryShapeReady(result.lex,
                                           OBJC3C_FRONTEND_STAGE_LEX) &&
         FrontendCApiStageSummaryShapeReady(result.parse,
                                           OBJC3C_FRONTEND_STAGE_PARSE) &&
         FrontendCApiStageSummaryShapeReady(result.sema,
                                           OBJC3C_FRONTEND_STAGE_SEMA) &&
         FrontendCApiStageSummaryShapeReady(result.lower,
                                           OBJC3C_FRONTEND_STAGE_LOWER) &&
         FrontendCApiStageSummaryShapeReady(result.emit,
                                           OBJC3C_FRONTEND_STAGE_EMIT);
}

bool ValidateFrontendCApiResultAccessors(
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const std::string &result_error_message,
    std::string &reason) {
  if (result.status != status) {
    reason = "compile status does not match result.status";
    return false;
  }
  if (status == OBJC3C_FRONTEND_STATUS_OK && result.success == 0u) {
    reason = "successful compile did not set result.success";
    return false;
  }
  if (status != OBJC3C_FRONTEND_STATUS_OK && result.success != 0u) {
    reason = "failing compile left result.success set";
    return false;
  }
  if (status == OBJC3C_FRONTEND_STATUS_OK && !result_error_message.empty()) {
    reason = "successful compile published a result-owned error message";
    return false;
  }
  if (status == OBJC3C_FRONTEND_STATUS_OK && !last_error.empty()) {
    reason = "successful compile published a context last_error";
    return false;
  }
  if (status != OBJC3C_FRONTEND_STATUS_OK && result_error_message.empty()) {
    reason = "failing compile published no result-owned error message";
    return false;
  }
  if (!last_error.empty() && !result_error_message.empty() &&
      last_error != result_error_message) {
    reason = "context last_error and result-owned error_message differ";
    return false;
  }
  return true;
}

namespace {

void AccumulateStageDiagnostics(
    const objc3c_frontend_c_stage_summary_t &summary,
    FrontendCApiDiagnosticTotals &totals) {
  totals.total += summary.diagnostics_total;
  totals.notes += summary.diagnostics_notes;
  totals.warnings += summary.diagnostics_warnings;
  totals.errors += summary.diagnostics_errors;
  totals.fatals += summary.diagnostics_fatals;
}

}  // namespace

FrontendCApiDiagnosticTotals BuildFrontendCApiDiagnosticTotals(
    const objc3c_frontend_c_compile_result_t &result) {
  FrontendCApiDiagnosticTotals totals;
  AccumulateStageDiagnostics(result.lex, totals);
  AccumulateStageDiagnostics(result.parse, totals);
  AccumulateStageDiagnostics(result.sema, totals);
  AccumulateStageDiagnostics(result.lower, totals);
  AccumulateStageDiagnostics(result.emit, totals);
  return totals;
}

const char *FrontendCApiStatusName(objc3c_frontend_c_status_t status) {
  switch (status) {
    case OBJC3C_FRONTEND_STATUS_OK:
      return "ok";
    case OBJC3C_FRONTEND_STATUS_DIAGNOSTICS:
      return "diagnostics";
    case OBJC3C_FRONTEND_STATUS_USAGE_ERROR:
      return "usage-error";
    case OBJC3C_FRONTEND_STATUS_EMIT_ERROR:
      return "emit-error";
    case OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR:
      return "internal-error";
    default:
      return "unknown";
  }
}

const char *HighestFrontendCApiDiagnosticSeverity(
    const FrontendCApiDiagnosticTotals &totals) {
  if (totals.fatals != 0) {
    return "fatal";
  }
  if (totals.errors != 0) {
    return "error";
  }
  if (totals.warnings != 0) {
    return "warning";
  }
  if (totals.notes != 0) {
    return "note";
  }
  return "none";
}

std::string LastAttemptedFrontendCApiStageName(
    const objc3c_frontend_c_compile_result_t &result) {
  if (result.emit.attempted != 0) {
    return "emit";
  }
  if (result.lower.attempted != 0) {
    return "lower";
  }
  if (result.sema.attempted != 0) {
    return "sema";
  }
  if (result.parse.attempted != 0) {
    return "parse";
  }
  if (result.lex.attempted != 0) {
    return "lex";
  }
  return "";
}

std::string BlockingFrontendCApiStageName(
    const objc3c_frontend_c_compile_result_t &result) {
  if (result.lex.diagnostics_errors != 0 ||
      result.lex.diagnostics_fatals != 0) {
    return "lex";
  }
  if (result.parse.diagnostics_errors != 0 ||
      result.parse.diagnostics_fatals != 0) {
    return "parse";
  }
  if (result.sema.diagnostics_errors != 0 ||
      result.sema.diagnostics_fatals != 0) {
    return "sema";
  }
  if (result.lower.diagnostics_errors != 0 ||
      result.lower.diagnostics_fatals != 0) {
    return "lower";
  }
  if (result.emit.diagnostics_errors != 0 ||
      result.emit.diagnostics_fatals != 0 || result.process_exit_code != 0) {
    return "emit";
  }
  return LastAttemptedFrontendCApiStageName(result);
}

int FrontendCApiExitCodeFromStatus(
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result) {
  switch (status) {
    case OBJC3C_FRONTEND_STATUS_OK:
      return 0;
    case OBJC3C_FRONTEND_STATUS_DIAGNOSTICS:
      return 1;
    case OBJC3C_FRONTEND_STATUS_USAGE_ERROR:
      return 2;
    case OBJC3C_FRONTEND_STATUS_EMIT_ERROR:
      return result.process_exit_code != 0 ? result.process_exit_code : 3;
    case OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR:
    default:
      return result.process_exit_code != 0 ? result.process_exit_code : 2;
  }
}

std::string ReadFrontendCApiLastError(
    const objc3c_frontend_c_context_t *context) {
  const std::size_t required =
      objc3c_frontend_c_copy_last_error(context, nullptr, 0);
  if (required == 0) {
    return "";
  }
  std::string message(required, '\0');
  const std::size_t written = objc3c_frontend_c_copy_last_error(
      context, message.data(), message.size());
  if (written == 0) {
    return "";
  }
  if (!message.empty() && message.back() == '\0') {
    message.pop_back();
  }
  return message;
}
