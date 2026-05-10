#include "tools/objc3c_frontend_c_api_runner_session_publication.h"

#include <iostream>

#include "tools/objc3c_frontend_c_api_runner_dump_actions.h"
#include "tools/objc3c_frontend_c_api_runner_summary_io.h"

bool PublishFrontendCApiRunnerSessionResult(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const FrontendCApiRunnerSessionResult &session_result,
    std::string &error) {
  if (!WriteFrontendCApiRunnerSummaryFile(summary_path,
                                          session_result.json,
                                          error)) {
    return false;
  }

  if (ShouldEmitFrontendCApiRunnerDumpActions(options)) {
    EmitFrontendCApiRunnerDumpActions(
        options,
        summary_path,
        *session_result.compile_result,
        session_result.status,
        session_result.public_result.result_error_message,
        session_result.artifact_paths.runtime_metadata_binary,
        session_result.json);
  } else {
    std::cout << "wrote summary: " << summary_path.generic_string() << "\n";
  }
  error.clear();
  return true;
}
