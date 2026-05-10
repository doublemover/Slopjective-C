#include "tools/objc3c_frontend_c_api_runner_result_contract_artifacts_internal.h"

bool ProbeFrontendCApiResultOwnedArtifactAvailability(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return objc3c_frontend_c_result_has_artifact(&result, artifact_kind) != 0u;
}
