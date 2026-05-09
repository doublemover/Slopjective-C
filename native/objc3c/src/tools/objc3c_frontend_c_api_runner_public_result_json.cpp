#include "tools/objc3c_frontend_c_api_runner_public_result_json.h"

#include <ostream>

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerPublicResultSummaryFields(
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
  out << "  \"paths\": {\n";
  out << "    \"summary\": \"" << EscapeJsonString(public_result.paths.summary)
      << "\",\n";
  out << "    \"diagnostics\": \""
      << EscapeJsonString(public_result.paths.diagnostics) << "\",\n";
  out << "    \"manifest\": \"" << EscapeJsonString(public_result.paths.manifest)
      << "\",\n";
  out << "    \"ir\": \"" << EscapeJsonString(public_result.paths.ir)
      << "\",\n";
  out << "    \"object\": \"" << EscapeJsonString(public_result.paths.object)
      << "\",\n";
  out << "    \"runtime_metadata_binary\": \""
      << EscapeJsonString(public_result.paths.runtime_metadata_binary)
      << "\"\n";
  out << "  },\n";
  out << "  \"last_error\": \"" << EscapeJsonString(public_result.last_error)
      << "\",\n";
  out << "  \"result_error_message\": \""
      << EscapeJsonString(public_result.result_error_message) << "\",\n";
  out << "  \"c_api_ownership\": {\n";
  out << "    \"result_owned_error_message\": "
      << (public_result.c_api_ownership.result_owned_error_message ? "true"
                                                                   : "false")
      << ",\n";
  out << "    \"diagnostics_path_borrowed\": "
      << (public_result.c_api_ownership.diagnostics_path_borrowed ? "true"
                                                                  : "false")
      << ",\n";
  out << "    \"manifest_path_borrowed\": "
      << (public_result.c_api_ownership.manifest_path_borrowed ? "true"
                                                               : "false")
      << ",\n";
  out << "    \"ir_path_borrowed\": "
      << (public_result.c_api_ownership.ir_path_borrowed ? "true" : "false")
      << ",\n";
  out << "    \"object_path_borrowed\": "
      << (public_result.c_api_ownership.object_path_borrowed ? "true"
                                                             : "false")
      << ",\n";
  out << "    \"runtime_metadata_path_borrowed\": "
      << (public_result.c_api_ownership.runtime_metadata_path_borrowed
              ? "true"
              : "false")
      << "\n";
  out << "  },\n";
}
