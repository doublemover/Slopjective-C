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
    const FrontendCApiRunnerDumpPublication dump_publication =
        BuildFrontendCApiRunnerDumpPublication(summary_path, session_result);
    EmitFrontendCApiRunnerDumpActions(options, dump_publication);
  } else {
    std::cout << "wrote summary: " << summary_path.generic_string() << "\n";
  }
  error.clear();
  return true;
}
