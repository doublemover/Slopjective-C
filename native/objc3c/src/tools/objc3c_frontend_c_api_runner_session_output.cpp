#include "tools/objc3c_frontend_c_api_runner_session_output.h"

#include <iostream>

#include "tools/objc3c_frontend_c_api_runner_dump_actions.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract.h"
#include "tools/objc3c_frontend_c_api_runner_result.h"
#include "tools/objc3c_frontend_c_api_runner_summary_io.h"
#include "tools/objc3c_frontend_c_api_runner_summary_json.h"

bool EmitFrontendCApiRunnerSessionOutput(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const FrontendCApiRunnerCompileSession &compile_session,
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

  const std::string runtime_metadata_binary_path_text =
      FrontendCApiResultArtifactPath(
          compile_session.result,
          OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA);
  const std::string summary_json = BuildFrontendCApiRunnerSummaryJson(
      options,
      summary_path,
      compile_session.status,
      compile_session.result,
      compile_session.last_error,
      compile_session.result_error_message,
      output_contract);
  if (!WriteFrontendCApiRunnerSummaryFile(summary_path,
                                          summary_json,
                                          error)) {
    return false;
  }

  if (ShouldEmitFrontendCApiRunnerDumpActions(options)) {
    EmitFrontendCApiRunnerDumpActions(
        options,
        summary_path,
        compile_session.result,
        compile_session.status,
        compile_session.result_error_message,
        runtime_metadata_binary_path_text,
        summary_json);
  } else {
    std::cout << "wrote summary: " << summary_path.generic_string() << "\n";
  }
  error.clear();
  return true;
}
