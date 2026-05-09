#include "tools/objc3c_frontend_c_api_runner_public_result_ownership.h"

namespace {

bool FrontendCApiRunnerResultHasArtifact(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return objc3c_frontend_c_result_has_artifact(&result, artifact_kind) != 0u;
}

FrontendCApiRunnerCArtifactOwnershipContract BuildArtifactOwnershipContract(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind,
    const char *name,
    bool required_by_runner) {
  FrontendCApiRunnerCArtifactOwnershipContract artifact;
  artifact.name = name;
  artifact.required_by_runner = required_by_runner;
  artifact.produced =
      FrontendCApiRunnerResultHasArtifact(result, artifact_kind);
  return artifact;
}

}  // namespace

FrontendCApiRunnerCOwnershipView BuildFrontendCApiRunnerCOwnershipView(
    const FrontendCApiRunnerOptions &options,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &result_error_message) {
  const bool ok_status = status == OBJC3C_FRONTEND_STATUS_OK;
  const bool pipeline_status =
      ok_status || status == OBJC3C_FRONTEND_STATUS_DIAGNOSTICS ||
      status == OBJC3C_FRONTEND_STATUS_EMIT_ERROR;
  FrontendCApiRunnerCOwnershipView ownership;
  ownership.error_message.present = !result_error_message.empty();
  ownership.error_message.null_contract =
      ok_status ? "must-be-null-on-success"
                : "must-be-present-and-nonempty-on-failure";
  ownership.artifacts = {
      BuildArtifactOwnershipContract(
          result,
          OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS,
          "diagnostics",
          pipeline_status),
      BuildArtifactOwnershipContract(
          result,
          OBJC3C_FRONTEND_ARTIFACT_MANIFEST,
          "manifest",
          options.emit_manifest && pipeline_status),
      BuildArtifactOwnershipContract(
          result,
          OBJC3C_FRONTEND_ARTIFACT_IR,
          "ir",
          (options.emit_ir || options.emit_object) && ok_status),
      BuildArtifactOwnershipContract(
          result,
          OBJC3C_FRONTEND_ARTIFACT_OBJECT,
          "object",
          options.emit_object && ok_status),
      BuildArtifactOwnershipContract(
          result,
          OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA,
          "runtime_metadata_binary",
          false),
  };
  return ownership;
}
