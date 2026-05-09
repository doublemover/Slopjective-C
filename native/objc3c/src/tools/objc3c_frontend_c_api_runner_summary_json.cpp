#include "tools/objc3c_frontend_c_api_runner_summary_json.h"

#include <sstream>

#include "io/objc3_json.h"
#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_json.h"
#include "tools/objc3c_frontend_c_api_runner_result.h"
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
  const char *backend_name =
      options.ir_object_backend == OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT
          ? "llvm-direct"
          : "clang";
  const std::string summary_path_text = summary_path.generic_string();
  const std::string runtime_metadata_binary_path_text =
      FrontendCApiResultArtifactPath(
          result,
          OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA);
  const std::string diagnostics_path_text =
      FrontendCApiResultArtifactPath(result,
                                     OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS);
  const std::string manifest_path_text =
      FrontendCApiResultArtifactPath(result, OBJC3C_FRONTEND_ARTIFACT_MANIFEST);
  const std::string ir_path_text =
      FrontendCApiResultArtifactPath(result, OBJC3C_FRONTEND_ARTIFACT_IR);
  const std::string object_path_text =
      FrontendCApiResultArtifactPath(result, OBJC3C_FRONTEND_ARTIFACT_OBJECT);

  std::ostringstream out;
  out << "{\n";
  out << "  \"mode\": \"objc3c-frontend-c-api-runner-v1\",\n";
  out << "  \"input_path\": \""
      << EscapeJsonString(options.input_path.generic_string()) << "\",\n";
  out << "  \"out_dir\": \"" << EscapeJsonString(options.out_dir.generic_string())
      << "\",\n";
  out << "  \"emit_prefix\": \"" << EscapeJsonString(options.emit_prefix)
      << "\",\n";
  out << "  \"ir_object_backend\": \"" << backend_name << "\",\n";
  out << "  \"status\": " << static_cast<unsigned>(status) << ",\n";
  out << "  \"process_exit_code\": " << result.process_exit_code << ",\n";
  out << "  \"success\": " << (result.success != 0 ? "true" : "false")
      << ",\n";
  out << "  \"semantic_skipped\": "
      << (result.semantic_skipped != 0 ? "true" : "false") << ",\n";
  out << "  \"paths\": {\n";
  out << "    \"summary\": \"" << EscapeJsonString(summary_path_text)
      << "\",\n";
  out << "    \"diagnostics\": \"" << EscapeJsonString(diagnostics_path_text)
      << "\",\n";
  out << "    \"manifest\": \"" << EscapeJsonString(manifest_path_text)
      << "\",\n";
  out << "    \"ir\": \"" << EscapeJsonString(ir_path_text) << "\",\n";
  out << "    \"object\": \"" << EscapeJsonString(object_path_text) << "\",\n";
  out << "    \"runtime_metadata_binary\": \""
      << EscapeJsonString(runtime_metadata_binary_path_text) << "\"\n";
  out << "  },\n";
  out << "  \"last_error\": \"" << EscapeJsonString(last_error) << "\",\n";
  out << "  \"result_error_message\": \""
      << EscapeJsonString(result_error_message) << "\",\n";
  out << "  \"c_api_ownership\": {\n";
  out << "    \"result_owned_error_message\": "
      << (!result_error_message.empty() ? "true" : "false") << ",\n";
  out << "    \"diagnostics_path_borrowed\": "
      << (objc3c_frontend_c_result_has_artifact(
              &result,
              OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS) != 0u
              ? "true"
              : "false")
      << ",\n";
  out << "    \"manifest_path_borrowed\": "
      << (objc3c_frontend_c_result_has_artifact(
              &result,
              OBJC3C_FRONTEND_ARTIFACT_MANIFEST) != 0u
              ? "true"
              : "false")
      << ",\n";
  out << "    \"ir_path_borrowed\": "
      << (objc3c_frontend_c_result_has_artifact(
              &result,
              OBJC3C_FRONTEND_ARTIFACT_IR) != 0u
              ? "true"
              : "false")
      << ",\n";
  out << "    \"object_path_borrowed\": "
      << (objc3c_frontend_c_result_has_artifact(
              &result,
              OBJC3C_FRONTEND_ARTIFACT_OBJECT) != 0u
              ? "true"
              : "false")
      << ",\n";
  out << "    \"runtime_metadata_path_borrowed\": "
      << (objc3c_frontend_c_result_has_artifact(
              &result,
              OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA) != 0u
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
      summary_path_text,
      result,
      status,
      result_error_message,
      runtime_metadata_binary_path_text);
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
      summary_path_text,
      runtime_metadata_binary_path_text);
  out << ",\n";
  out << "  \"output_contract\": ";
  WriteFrontendCApiRunnerOutputContractJson(out, "  ", output_contract);
  out << "\n";
  out << "}\n";
  return out.str();
}
