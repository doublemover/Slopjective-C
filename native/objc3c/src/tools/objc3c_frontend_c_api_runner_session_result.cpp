#include "tools/objc3c_frontend_c_api_runner_session_result.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract.h"
#include "tools/objc3c_frontend_c_api_runner_summary_json.h"

bool BuildFrontendCApiRunnerSessionResult(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const FrontendCApiRunnerCompileSession &compile_session,
    FrontendCApiRunnerSessionResult &session_result,
    std::string &error) {
  FrontendCApiRunnerOutputContract output_contract;
  if (!BuildFrontendCApiRunnerOutputContract(
          options,
          summary_path,
          compile_session.result,
          output_contract,
          error)) {
    return false;
  }

  session_result.compile_result = &compile_session.result;
  session_result.status = compile_session.status;
  session_result.output_contract = output_contract;
  session_result.artifact_paths = BuildFrontendCApiRunnerArtifactPathView(
      compile_session.result,
      summary_path);
  session_result.public_result = BuildFrontendCApiRunnerPublicResultView(
      options,
      session_result.artifact_paths,
      compile_session.status,
      compile_session.result,
      compile_session.error_snapshot);
  session_result.json = BuildFrontendCApiRunnerSummaryJson(
      options,
      compile_session.result,
      compile_session.status,
      session_result.public_result,
      session_result.output_contract);
  error.clear();
  return true;
}
