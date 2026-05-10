#include "tools/objc3c_frontend_c_api_runner_compile_session_run.h"

#include "tools/objc3c_frontend_c_api_runner_compile_session_run_internal.h"

bool RunFrontendCApiRunnerCompileSession(
    const FrontendCApiRunnerOptions &options,
    FrontendCApiRunnerCompileSession &session,
    std::string &error) {
  FrontendCApiContextOwner context;
  if (!ExecuteFrontendCApiRunnerCompileContext(options,
                                               session,
                                               context,
                                               error)) {
    return false;
  }

  CaptureFrontendCApiRunnerCompileResultSnapshots(context, session);
  return PublishFrontendCApiRunnerCompileSessionResult(options, session, error);
}
