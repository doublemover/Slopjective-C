#include "tools/objc3c_frontend_c_api_runner_session_summary.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract.h"
#include "tools/objc3c_frontend_c_api_runner_summary_json.h"

bool BuildFrontendCApiRunnerSessionSummary(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const FrontendCApiRunnerCompileSession &compile_session,
    FrontendCApiRunnerSessionSummary &summary,
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

  summary.artifact_paths = BuildFrontendCApiRunnerArtifactPathView(
      compile_session.result,
      summary_path);
  summary.json = BuildFrontendCApiRunnerSummaryJson(
      options,
      summary_path,
      summary.artifact_paths,
      compile_session.status,
      compile_session.result,
      compile_session.last_error,
      compile_session.result_error_message_snapshot,
      output_contract);
  error.clear();
  return true;
}
