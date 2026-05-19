#include "tools/objc3c_frontend_c_api_runner_public_result_json_status.h"

#include <ostream>

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerPublicResultStatusJsonRows(
    std::ostream &out,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerPublicResultView &public_result) {
  out << "  \"mode\": \"objc3c-frontend-c-api-runner-v1\",\n";
  out << "  \"input_path\": \""
      << EscapeJsonString(options.input_path.generic_string()) << "\",\n";
  out << "  \"out_dir\": \"" << EscapeJsonString(options.out_dir.generic_string())
      << "\",\n";
  out << "  \"emit_prefix\": \"" << EscapeJsonString(options.emit_prefix)
      << "\",\n";
  out << "  \"ir_object_backend\": \"" << public_result.backend_name << "\",\n";
  out << "  \"status\": " << public_result.status_code << ",\n";
  out << "  \"process_exit_code\": " << public_result.process_exit_code
      << ",\n";
  out << "  \"success\": " << (public_result.success ? "true" : "false")
      << ",\n";
  out << "  \"semantic_skipped\": "
      << (public_result.semantic_skipped ? "true" : "false") << ",\n";
}
