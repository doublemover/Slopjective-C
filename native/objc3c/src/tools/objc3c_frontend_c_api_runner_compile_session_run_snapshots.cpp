#include "tools/objc3c_frontend_c_api_runner_compile_session_run_internal.h"

#include "tools/objc3c_frontend_c_api_runner_result_error_snapshot.h"

void CaptureFrontendCApiRunnerCompileResultSnapshots(
    const FrontendCApiContextOwner &context,
    FrontendCApiRunnerCompileSession &session) {
  session.error_snapshot =
      CaptureFrontendCApiRunnerResultErrorSnapshot(context.get(),
                                                   session.result);
}
