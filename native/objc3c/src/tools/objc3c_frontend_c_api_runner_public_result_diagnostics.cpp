#include "tools/objc3c_frontend_c_api_runner_public_result_diagnostics.h"

FrontendCApiRunnerPublicResultDiagnostics
BuildFrontendCApiRunnerPublicResultDiagnostics(
    const FrontendCApiRunnerResultErrorSnapshot &error_snapshot) {
  FrontendCApiRunnerPublicResultDiagnostics diagnostics;
  diagnostics.last_error = error_snapshot.last_error;
  diagnostics.result_error_message = error_snapshot.result_error_message.text;
  diagnostics.result_error_message_present =
      error_snapshot.result_error_message.present;
  return diagnostics;
}
