#include "tools/objc3c_frontend_c_api_runner_result_contract_artifacts_internal.h"

#include "tools/objc3c_frontend_c_api_runner_c_string.h"

FrontendCApiRunnerResultArtifactPathProbe
ProbeFrontendCApiResultOwnedArtifactPath(
    const objc3c_frontend_c_compile_result_t &result,
    const FrontendCApiRunnerCArtifactRequirement &requirement) {
  FrontendCApiRunnerResultArtifactPathProbe probe;
  probe.path =
      FrontendCApiResultArtifactPathSnapshot(result, requirement.artifact_kind);
  return probe;
}
