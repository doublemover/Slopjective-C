#include "tools/objc3c_frontend_c_api_runner_compile_session.h"

#include "tools/objc3c_frontend_c_api_runner_invocation.h"

FrontendCApiRunnerCompileSession::FrontendCApiRunnerCompileSession()
    : result_guard{&result} {}

bool RunFrontendCApiRunnerCompileSession(
    const FrontendCApiRunnerOptions &options,
    FrontendCApiRunnerCompileSession &session,
    std::string &error) {
  FrontendCApiContextOwner context;
  if (!context.valid()) {
    error = "failed to allocate frontend context";
    return false;
  }

  const FrontendCApiRunnerCompileInvocation compile_invocation(options);
  session.status = objc3c_frontend_c_compile_file(
      context.get(),
      compile_invocation.compile_options(),
      &session.result);
  session.last_error = ReadFrontendCApiLastError(context.get());
  session.result_error_message =
      FrontendCApiResultErrorMessage(session.result);

  std::string accessor_contract_error;
  if (!ValidateFrontendCApiResultAccessors(
          session.status,
          session.result,
          session.last_error,
          session.result_error_message,
          accessor_contract_error)) {
    error = "frontend C API accessor contract fail-closed: ";
    error += accessor_contract_error;
    return false;
  }

  session.exit_code =
      FrontendCApiExitCodeFromStatus(session.status, session.result);
  error.clear();
  return true;
}
