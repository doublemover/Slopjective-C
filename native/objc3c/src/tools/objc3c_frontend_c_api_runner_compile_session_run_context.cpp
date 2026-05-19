#include "tools/objc3c_frontend_c_api_runner_compile_session_run_internal.h"

#include "tools/objc3c_frontend_c_api_runner_invocation.h"

bool ExecuteFrontendCApiRunnerCompileContext(
    const FrontendCApiRunnerOptions &options,
    FrontendCApiRunnerCompileSession &session,
    FrontendCApiContextOwner &context,
    std::string &error) {
  if (!context.valid()) {
    error = "failed to allocate frontend context";
    return false;
  }

  const FrontendCApiRunnerCompileInvocation compile_invocation(options);
  session.status = objc3c_frontend_c_compile_file_owned(
      context.get(),
      compile_invocation.compile_options(),
      session.compile_result.out_param());
  if (!session.compile_result.valid()) {
    error = "failed to allocate frontend owned result handle";
    return false;
  }
  return true;
}
