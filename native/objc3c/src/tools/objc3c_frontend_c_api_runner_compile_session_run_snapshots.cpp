#include "tools/objc3c_frontend_c_api_runner_compile_session_run_internal.h"

#include "tools/objc3c_frontend_c_api_runner_c_string.h"

void CaptureFrontendCApiRunnerCompileResultSnapshots(
    const FrontendCApiContextOwner &context,
    FrontendCApiRunnerCompileSession &session) {
  session.last_error = ReadFrontendCApiLastError(context.get());
  session.result_error_message_snapshot =
      FrontendCApiResultErrorMessageSnapshot(session.result);
  session.result_error_message = session.result_error_message_snapshot.text;
}
