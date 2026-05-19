#include "tools/objc3c_frontend_c_api_runner_public_result_ownership_artifact_contract.h"

#include "tools/objc3c_frontend_c_api_runner_result_artifact_snapshot.h"

FrontendCApiRunnerCArtifactOwnershipContract
BuildFrontendCApiRunnerCArtifactOwnershipContract(
    const objc3c_frontend_c_compile_result_t &result,
    const FrontendCApiRunnerCArtifactRequirement &requirement) {
  const FrontendCApiRunnerResultArtifactSnapshot snapshot =
      CaptureFrontendCApiRunnerResultArtifactSnapshot(result,
                                                      requirement.artifact_kind);
  FrontendCApiRunnerCArtifactOwnershipContract artifact;
  artifact.name = requirement.name;
  artifact.required_by_runner = requirement.required_by_runner;
  artifact.produced = snapshot.produced;
  artifact.path_snapshot_present = snapshot.path.present;
  return artifact;
}
