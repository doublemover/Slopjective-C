#include "tools/objc3c_frontend_c_api_runner_public_result.h"

namespace {

bool FrontendCApiResultHasArtifact(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return objc3c_frontend_c_result_has_artifact(&result, artifact_kind) != 0u;
}

}  // namespace

FrontendCApiRunnerPublicResultView BuildFrontendCApiRunnerPublicResultView(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const std::string &result_error_message) {
  FrontendCApiRunnerPublicResultView view;
  view.backend_name =
      options.ir_object_backend == OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT
          ? "llvm-direct"
          : "clang";
  view.status_code = static_cast<unsigned>(status);
  view.process_exit_code = result.process_exit_code;
  view.success = result.success != 0;
  view.semantic_skipped = result.semantic_skipped != 0;
  view.paths = BuildFrontendCApiRunnerArtifactPathView(result, summary_path);
  view.last_error = last_error;
  view.result_error_message = result_error_message;
  view.c_api_ownership.result_owned_error_message =
      !result_error_message.empty();
  view.c_api_ownership.diagnostics_path_borrowed =
      FrontendCApiResultHasArtifact(result,
                                    OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS);
  view.c_api_ownership.manifest_path_borrowed =
      FrontendCApiResultHasArtifact(result,
                                    OBJC3C_FRONTEND_ARTIFACT_MANIFEST);
  view.c_api_ownership.ir_path_borrowed =
      FrontendCApiResultHasArtifact(result, OBJC3C_FRONTEND_ARTIFACT_IR);
  view.c_api_ownership.object_path_borrowed =
      FrontendCApiResultHasArtifact(result, OBJC3C_FRONTEND_ARTIFACT_OBJECT);
  view.c_api_ownership.runtime_metadata_path_borrowed =
      FrontendCApiResultHasArtifact(
          result,
          OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA);
  return view;
}
