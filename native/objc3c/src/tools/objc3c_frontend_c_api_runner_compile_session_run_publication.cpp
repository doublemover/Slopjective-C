#include "tools/objc3c_frontend_c_api_runner_compile_session_run_internal.h"

#include "tools/objc3c_frontend_c_api_runner_result_contract.h"
#include "tools/objc3c_frontend_c_api_runner_status_mapping.h"

bool PublishFrontendCApiRunnerCompileSessionResult(
    const FrontendCApiRunnerOptions &options,
    FrontendCApiRunnerCompileSession &session,
    std::string &error) {
  std::string accessor_contract_error;
  if (!ValidateFrontendCApiResultAccessors(
          options,
          session.status,
          session.compile_result.view(),
          session.error_snapshot,
          accessor_contract_error)) {
    error = "frontend C API accessor contract fail-closed: ";
    error += accessor_contract_error;
    return false;
  }

  session.exit_code =
      FrontendCApiExitCodeFromStatus(session.status,
                                     session.compile_result.view());
  error.clear();
  return true;
}
