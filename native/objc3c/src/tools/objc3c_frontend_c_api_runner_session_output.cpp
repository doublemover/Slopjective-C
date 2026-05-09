#include "tools/objc3c_frontend_c_api_runner_session_output.h"

#include "tools/objc3c_frontend_c_api_runner_session_publication.h"
#include "tools/objc3c_frontend_c_api_runner_session_summary.h"

bool EmitFrontendCApiRunnerSessionOutput(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const FrontendCApiRunnerCompileSession &compile_session,
    std::string &error) {
  FrontendCApiRunnerSessionSummary summary;
  if (!BuildFrontendCApiRunnerSessionSummary(
          options,
          summary_path,
          compile_session,
          summary,
          error)) {
    return false;
  }

  return PublishFrontendCApiRunnerSessionSummary(
      options,
      summary_path,
      compile_session,
      summary,
      error);
}
