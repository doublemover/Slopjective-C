#include "tools/objc3c_frontend_c_api_runner_public_result_ownership.h"

namespace {

bool FrontendCApiResultHasArtifact(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return objc3c_frontend_c_result_has_artifact(&result, artifact_kind) != 0u;
}

}  // namespace

FrontendCApiRunnerCOwnershipView BuildFrontendCApiRunnerCOwnershipView(
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &result_error_message) {
  FrontendCApiRunnerCOwnershipView ownership;
  ownership.result_owned_error_message = !result_error_message.empty();
  ownership.diagnostics_path_borrowed =
      FrontendCApiResultHasArtifact(result,
                                    OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS);
  ownership.manifest_path_borrowed =
      FrontendCApiResultHasArtifact(result,
                                    OBJC3C_FRONTEND_ARTIFACT_MANIFEST);
  ownership.ir_path_borrowed =
      FrontendCApiResultHasArtifact(result, OBJC3C_FRONTEND_ARTIFACT_IR);
  ownership.object_path_borrowed =
      FrontendCApiResultHasArtifact(result, OBJC3C_FRONTEND_ARTIFACT_OBJECT);
  ownership.runtime_metadata_path_borrowed =
      FrontendCApiResultHasArtifact(
          result,
          OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA);
  return ownership;
}
