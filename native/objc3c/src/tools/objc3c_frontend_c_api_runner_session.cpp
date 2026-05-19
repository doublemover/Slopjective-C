#include "tools/objc3c_frontend_c_api_runner_session.h"

#include <filesystem>
#include <iostream>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_compile_session_run.h"
#include "tools/objc3c_frontend_c_api_runner_session_output.h"
#include "tools/objc3c_frontend_c_api_runner_summary_path.h"

int RunFrontendCApiRunnerSession(const FrontendCApiRunnerOptions &options) {
  FrontendCApiRunnerCompileSession compile_session;
  std::string session_error;
  if (!RunFrontendCApiRunnerCompileSession(
          options,
          compile_session,
          session_error)) {
    std::cerr << session_error << "\n";
    return 2;
  }

  const std::filesystem::path summary_path =
      BuildFrontendCApiRunnerSummaryPath(options);
  std::string output_error;
  if (!EmitFrontendCApiRunnerSessionOutput(
          options,
          summary_path,
          compile_session,
          output_error)) {
    std::cerr << output_error << "\n";
    return 2;
  }

  if (!compile_session.error_snapshot.last_error.empty()) {
    std::cerr << compile_session.error_snapshot.last_error << "\n";
  }
  return compile_session.exit_code;
}
