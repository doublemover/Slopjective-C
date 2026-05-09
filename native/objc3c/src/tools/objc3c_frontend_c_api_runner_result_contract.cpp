#include "tools/objc3c_frontend_c_api_runner_result_contract.h"

#include "tools/objc3c_frontend_c_api_runner_c_string.h"

namespace {

bool ValidateFrontendCApiResultOwnedArtifactPath(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind,
    const char *artifact_name,
    bool required,
    std::string &reason) {
  const FrontendCApiRunnerStringSnapshot path =
      FrontendCApiResultArtifactPathSnapshot(result, artifact_kind);
  const bool has_artifact =
      objc3c_frontend_c_result_has_artifact(&result, artifact_kind) != 0u;
  if (path.present && path.text.empty()) {
    reason = std::string("result-owned ") + artifact_name +
             " artifact path is present but empty";
    return false;
  }
  if (has_artifact != path.present) {
    reason = std::string("result-owned ") + artifact_name +
             " artifact path presence disagrees with result_has_artifact";
    return false;
  }
  if (required && !has_artifact) {
    reason = std::string("required result-owned ") + artifact_name +
             " artifact path is missing";
    return false;
  }
  return true;
}

}  // namespace

bool ValidateFrontendCApiResultAccessors(
    const FrontendCApiRunnerOptions &options,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const std::string &result_error_message,
    std::string &reason) {
  const bool ok_status = status == OBJC3C_FRONTEND_STATUS_OK;
  const bool pipeline_status =
      ok_status || status == OBJC3C_FRONTEND_STATUS_DIAGNOSTICS ||
      status == OBJC3C_FRONTEND_STATUS_EMIT_ERROR;
  const FrontendCApiRunnerStringSnapshot result_error =
      FrontendCApiResultErrorMessageSnapshot(result);
  if (result.status != status) {
    reason = "compile status does not match result.status";
    return false;
  }
  if (ok_status && result.success == 0u) {
    reason = "successful compile did not set result.success";
    return false;
  }
  if (!ok_status && result.success != 0u) {
    reason = "failing compile left result.success set";
    return false;
  }
  if (ok_status && result_error.present) {
    reason = "successful compile published a result-owned error message";
    return false;
  }
  if (ok_status && !last_error.empty()) {
    reason = "successful compile published a context last_error";
    return false;
  }
  if (!ok_status && !result_error.present) {
    reason = "failing compile published no result-owned error message";
    return false;
  }
  if (result_error.present && result_error.text.empty()) {
    reason = "result-owned error message is present but empty";
    return false;
  }
  if (result_error.text != result_error_message) {
    reason = "result-owned error_message snapshot differs from accessor text";
    return false;
  }
  if (!last_error.empty() && !result_error_message.empty() &&
      last_error != result_error_message) {
    reason = "context last_error and result-owned error_message differ";
    return false;
  }
  if (!ValidateFrontendCApiResultOwnedArtifactPath(
          result,
          OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS,
          "diagnostics",
          pipeline_status,
          reason)) {
    return false;
  }
  if (!ValidateFrontendCApiResultOwnedArtifactPath(
          result,
          OBJC3C_FRONTEND_ARTIFACT_MANIFEST,
          "manifest",
          options.emit_manifest && pipeline_status,
          reason)) {
    return false;
  }
  if (!ValidateFrontendCApiResultOwnedArtifactPath(
          result,
          OBJC3C_FRONTEND_ARTIFACT_IR,
          "ir",
          (options.emit_ir || options.emit_object) && ok_status,
          reason)) {
    return false;
  }
  if (!ValidateFrontendCApiResultOwnedArtifactPath(
          result,
          OBJC3C_FRONTEND_ARTIFACT_OBJECT,
          "object",
          options.emit_object && ok_status,
          reason)) {
    return false;
  }
  if (!ValidateFrontendCApiResultOwnedArtifactPath(
          result,
          OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA,
          "runtime_metadata",
          false,
          reason)) {
    return false;
  }
  return true;
}
