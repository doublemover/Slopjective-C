#include "tools/objc3c_frontend_c_api_runner_public_result.h"

#include "tools/objc3c_frontend_c_api_runner_public_result_backend.h"

FrontendCApiRunnerPublicResultView BuildFrontendCApiRunnerPublicResultView(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const std::string &result_error_message) {
  FrontendCApiRunnerPublicResultView view;
  view.backend_name = FrontendCApiRunnerPublicResultBackendName(options);
  view.status_code = static_cast<unsigned>(status);
  view.process_exit_code = result.process_exit_code;
  view.success = result.success != 0;
  view.semantic_skipped = result.semantic_skipped != 0;
  view.paths = BuildFrontendCApiRunnerArtifactPathView(result, summary_path);
  view.last_error = last_error;
  view.result_error_message = result_error_message;
  view.c_api_ownership = BuildFrontendCApiRunnerCOwnershipView(
      options,
      status,
      result,
      result_error_message);
  return view;
}
