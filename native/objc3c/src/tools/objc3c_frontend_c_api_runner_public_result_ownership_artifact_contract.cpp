#include "tools/objc3c_frontend_c_api_runner_public_result_ownership_artifact_contract.h"

namespace {

bool FrontendCApiRunnerResultHasArtifact(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return objc3c_frontend_c_result_has_artifact(&result, artifact_kind) != 0u;
}

}  // namespace

FrontendCApiRunnerCArtifactOwnershipContract
BuildFrontendCApiRunnerCArtifactOwnershipContract(
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
