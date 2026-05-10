#include "tools/objc3c_frontend_c_api_runner_public_result_json_diagnostics.h"

#include <ostream>

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerPublicResultDiagnosticJsonRows(
    std::ostream &out,
    const FrontendCApiRunnerPublicResultView &public_result) {
  const FrontendCApiRunnerPublicResultDiagnostics &diagnostics =
      public_result.diagnostics;
  out << "  \"last_error\": \"" << EscapeJsonString(diagnostics.last_error)
      << "\",\n";
  out << "  \"result_error_message\": \""
      << EscapeJsonString(diagnostics.result_error_message) << "\",\n";
  out << "  \"result_error_message_present\": "
      << (diagnostics.result_error_message_present ? "true" : "false")
      << ",\n";
}
