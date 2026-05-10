#include "tools/objc3c_frontend_c_api_runner_public_result_json_diagnostics.h"

#include <ostream>

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerPublicResultDiagnosticJsonRows(
    std::ostream &out,
    const FrontendCApiRunnerPublicResultView &public_result) {
  out << "  \"last_error\": \"" << EscapeJsonString(public_result.last_error)
      << "\",\n";
  out << "  \"result_error_message\": \""
      << EscapeJsonString(public_result.result_error_message) << "\",\n";
  out << "  \"result_error_message_present\": "
      << (public_result.result_error_message_present ? "true" : "false")
      << ",\n";
}
