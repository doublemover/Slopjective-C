#include "tools/objc3c_frontend_c_api_runner_session_output.h"

#include "tools/objc3c_frontend_c_api_runner_session_publication.h"
#include "tools/objc3c_frontend_c_api_runner_session_result.h"

bool EmitFrontendCApiRunnerSessionOutput(
    const FrontendCApiRunnerOptions &options,
    const std::filesystem::path &summary_path,
    const FrontendCApiRunnerCompileSession &compile_session,
    std::string &error) {
  FrontendCApiRunnerSessionResult session_result;
  if (!BuildFrontendCApiRunnerSessionResult(
          options,
          summary_path,
          compile_session,
          session_result,
          error)) {
    return false;
  }

  return PublishFrontendCApiRunnerSessionResult(
      options,
      summary_path,
      session_result,
      error);
}
