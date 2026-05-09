#include "tools/objc3c_frontend_c_api_runner_summary_json.h"

#include <sstream>

#include "io/objc3_json.h"
#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_json.h"
#include "tools/objc3c_frontend_c_api_runner_public_result.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json.h"
#include "tools/objc3c_frontend_c_api_runner_stage_trace_json.h"

namespace fs = std::filesystem;

using objc3::io::EscapeJsonString;

std::string BuildFrontendCApiRunnerSummaryJson(
    const FrontendCApiRunnerOptions &options,
    const fs::path &summary_path,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const std::string &result_error_message,
    const FrontendCApiRunnerOutputContract &output_contract) {
  const FrontendCApiRunnerPublicResultView public_result =
      BuildFrontendCApiRunnerPublicResultView(
          options,
          summary_path,
          status,
          result,
          last_error,
          result_error_message);

  std::ostringstream out;
  out << "{\n";
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
  out << "  \"stages\": {\n";
  WriteFrontendCApiRunnerStageSummaryJson(out, "lex", result.lex, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "parse", result.parse, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "sema", result.sema, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "lower", result.lower, true);
  WriteFrontendCApiRunnerStageSummaryJson(out, "emit", result.emit, false);
  out << "  },\n";
  out << "  \"observability\": ";
  WriteFrontendCApiRunnerObservabilityJson(
      out,
      "  ",
      public_result.paths.summary,
      result,
      status,
      result_error_message,
      public_result.paths.runtime_metadata_binary);
  out << ",\n";
  out << "  \"runtime_inspector\": ";
  WriteFrontendCApiRunnerRuntimeInspectorJson(out, "  ", options, result);
  out << ",\n";
  out << "  \"bonus_experiences\": ";
  WriteFrontendCApiRunnerBonusExperiencesJson(
      out,
      "  ",
      options,
      result,
      public_result.paths.summary,
      public_result.paths.runtime_metadata_binary);
  out << ",\n";
  out << "  \"output_contract\": ";
  WriteFrontendCApiRunnerOutputContractJson(out, "  ", output_contract);
  out << "\n";
  out << "}\n";
  return out.str();
}
