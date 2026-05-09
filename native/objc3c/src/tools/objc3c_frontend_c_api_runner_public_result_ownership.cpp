#include "tools/objc3c_frontend_c_api_runner_public_result_ownership.h"

#include <cstddef>

namespace {

bool FrontendCApiRunnerResultHasArtifact(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return objc3c_frontend_c_result_has_artifact(&result, artifact_kind) != 0u;
}

FrontendCApiRunnerCArtifactOwnershipContract BuildArtifactOwnershipContract(
    const objc3c_frontend_c_compile_result_t &result,
    const FrontendCApiRunnerCArtifactRequirement &requirement) {
  FrontendCApiRunnerCArtifactOwnershipContract artifact;
  artifact.name = requirement.name;
  artifact.required_by_runner = requirement.required_by_runner;
  artifact.produced =
      FrontendCApiRunnerResultHasArtifact(result, requirement.artifact_kind);
  artifact.path_snapshot_present =
      FrontendCApiResultArtifactPathSnapshot(
          result,
          requirement.artifact_kind)
          .present;
  return artifact;
}

}  // namespace

std::array<FrontendCApiRunnerCArtifactRequirement, 5>
BuildFrontendCApiRunnerCArtifactRequirements(
    const FrontendCApiRunnerOptions &options,
    objc3c_frontend_c_status_t status) {
  const bool ok_status = status == OBJC3C_FRONTEND_STATUS_OK;
  const bool pipeline_status =
      ok_status || status == OBJC3C_FRONTEND_STATUS_DIAGNOSTICS ||
      status == OBJC3C_FRONTEND_STATUS_EMIT_ERROR;
  return {{
      {OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS,
       "diagnostics",
       pipeline_status},
      {OBJC3C_FRONTEND_ARTIFACT_MANIFEST,
       "manifest",
       options.emit_manifest && pipeline_status},
      {OBJC3C_FRONTEND_ARTIFACT_IR,
       "ir",
       (options.emit_ir || options.emit_object) && ok_status},
      {OBJC3C_FRONTEND_ARTIFACT_OBJECT,
       "object",
       options.emit_object && ok_status},
      {OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA,
       "runtime_metadata_binary",
       false},
  }};
}

FrontendCApiRunnerCOwnershipView BuildFrontendCApiRunnerCOwnershipView(
    const FrontendCApiRunnerOptions &options,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const FrontendCApiRunnerStringSnapshot &result_error_message) {
  const bool ok_status = status == OBJC3C_FRONTEND_STATUS_OK;
  FrontendCApiRunnerCOwnershipView ownership;
  ownership.error_message.present = result_error_message.present;
  ownership.error_message.null_contract =
      ok_status ? "must-be-null-on-success"
                : "must-be-present-and-nonempty-on-failure";
  const std::array<FrontendCApiRunnerCArtifactRequirement, 5> requirements =
      BuildFrontendCApiRunnerCArtifactRequirements(options, status);
  for (std::size_t index = 0; index < requirements.size(); ++index) {
    ownership.artifacts[index] =
        BuildArtifactOwnershipContract(result, requirements[index]);
  }
  return ownership;
}
