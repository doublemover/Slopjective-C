#include "tools/objc3c_frontend_c_api_runner_c_string.h"

FrontendCApiRunnerStringSnapshot FrontendCApiResultArtifactPathSnapshot(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return SnapshotOptionalFrontendCApiString(
      objc3c_frontend_c_result_artifact_path(&result, artifact_kind));
}

std::string FrontendCApiResultArtifactPath(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return FrontendCApiResultArtifactPathSnapshot(result, artifact_kind).text;
}
