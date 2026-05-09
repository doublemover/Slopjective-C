#include "tools/objc3c_frontend_c_api_runner_compile_session_run.h"

#include "tools/objc3c_frontend_c_api_runner_compile_options.h"
#include "tools/objc3c_frontend_c_api_runner_context.h"

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
  session.result_error_message_snapshot =
      FrontendCApiResultErrorMessageSnapshot(session.result);
  session.result_error_message = session.result_error_message_snapshot.text;

  std::string accessor_contract_error;
  if (!ValidateFrontendCApiResultAccessors(
          options,
          session.status,
          session.result,
          session.last_error,
          session.result_error_message_snapshot,
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
