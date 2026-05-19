#include "tools/objc3c_frontend_c_api_runner_result_artifact_snapshot.h"

FrontendCApiRunnerResultArtifactSnapshot
CaptureFrontendCApiRunnerResultArtifactSnapshot(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  FrontendCApiRunnerResultArtifactSnapshot snapshot;
  snapshot.produced =
      objc3c_frontend_c_result_has_artifact(&result, artifact_kind) != 0u;
  snapshot.path = SnapshotOptionalFrontendCApiString(
      objc3c_frontend_c_result_artifact_path(&result, artifact_kind));
  return snapshot;
}

std::string FrontendCApiRunnerResultArtifactPathText(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return CaptureFrontendCApiRunnerResultArtifactSnapshot(result, artifact_kind)
      .path.text;
}
