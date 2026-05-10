#include "tools/objc3c_frontend_c_api_runner_session_publication.h"

#include <iostream>

#include "tools/objc3c_frontend_c_api_runner_dump_actions.h"
#include "tools/objc3c_frontend_c_api_runner_result_error_snapshot.h"
#include "tools/objc3c_frontend_c_api_runner_summary_io.h"

bool PublishFrontendCApiRunnerSessionSummary(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const FrontendCApiRunnerCompileSession &compile_session,
    const FrontendCApiRunnerSessionSummary &summary,
    std::string &error) {
  if (!WriteFrontendCApiRunnerSummaryFile(summary_path, summary.json, error)) {
    return false;
  }

  if (ShouldEmitFrontendCApiRunnerDumpActions(options)) {
    EmitFrontendCApiRunnerDumpActions(
        options,
        summary_path,
        compile_session.result,
        compile_session.status,
        FrontendCApiRunnerResultErrorMessageText(
            compile_session.error_snapshot),
        summary.artifact_paths.runtime_metadata_binary,
        summary.json);
  } else {
    std::cout << "wrote summary: " << summary_path.generic_string() << "\n";
  }
  error.clear();
  return true;
}
