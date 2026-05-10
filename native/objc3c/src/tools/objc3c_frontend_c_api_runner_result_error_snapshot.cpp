#include "tools/objc3c_frontend_c_api_runner_result_error_snapshot.h"

#include "tools/objc3c_frontend_c_api_runner_c_string.h"

FrontendCApiRunnerResultErrorSnapshot
CaptureFrontendCApiRunnerResultErrorSnapshot(
    const objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_result_t &result) {
  FrontendCApiRunnerResultErrorSnapshot snapshot;
  snapshot.last_error = ReadFrontendCApiLastError(context);
  snapshot.result_error_message = FrontendCApiResultErrorMessageSnapshot(result);
  return snapshot;
}
