#include "tools/objc3c_frontend_c_api_runner_public_result_json.h"

#include <ostream>
#include <sstream>

#include "io/json/json_writer.h"
#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;
using objc3::io::json::JsonArrayWriter;
using objc3::io::json::JsonObjectWriter;

namespace {

std::string RenderFrontendCApiRunnerPathsJson(
    const FrontendCApiRunnerArtifactPathView &paths) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("summary", paths.summary);
  object.StringField("diagnostics", paths.diagnostics);
  object.StringField("manifest", paths.manifest);
  object.StringField("ir", paths.ir);
  object.StringField("object", paths.object);
  object.StringField("runtime_metadata_binary", paths.runtime_metadata_binary);
  object.End();
  return out.str();
}

std::string RenderFrontendCApiRunnerStringOwnershipJson(
    const FrontendCApiRunnerCStringOwnershipContract &contract) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.BoolField("present", contract.present);
  object.StringField("storage_owner", contract.storage_owner);
  object.StringField("accessor_view", contract.accessor_view);
  object.StringField("release_function", contract.release_function);
  object.StringField("null_contract", contract.null_contract);
  object.End();
  return out.str();
}

std::string RenderFrontendCApiRunnerArtifactOwnershipJson(
    const FrontendCApiRunnerCArtifactOwnershipContract &contract) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("name", contract.name);
  object.BoolField("required_by_runner", contract.required_by_runner);
  object.BoolField("produced", contract.produced);
  object.StringField("storage_owner", contract.storage_owner);
  object.StringField("accessor_view", contract.accessor_view);
  object.StringField("release_function", contract.release_function);
  object.StringField("null_contract", contract.null_contract);
  object.End();
  return out.str();
}

std::string RenderFrontendCApiRunnerArtifactOwnershipArrayJson(
    const FrontendCApiRunnerCOwnershipView &ownership) {
  std::ostringstream out;
  JsonArrayWriter array(out);
  for (const FrontendCApiRunnerCArtifactOwnershipContract &artifact :
       ownership.artifacts) {
    array.RawJsonValue(RenderFrontendCApiRunnerArtifactOwnershipJson(artifact));
  }
  array.End();
  return out.str();
}

std::string RenderFrontendCApiRunnerCOwnershipJson(
    const FrontendCApiRunnerCOwnershipView &ownership) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("result_storage_owner", ownership.result_storage_owner);
  object.StringField("result_release_function",
                     ownership.result_release_function);
  object.StringField("result_release_timing", ownership.result_release_timing);
  object.StringField("context_lifetime", ownership.context_lifetime);
  object.StringField("compile_options_lifetime",
                     ownership.compile_options_lifetime);
  object.StringField("standalone_string_release_function",
                     ownership.standalone_string_release_function);
  object.RawJsonField(
      "error_message",
      RenderFrontendCApiRunnerStringOwnershipJson(ownership.error_message));
  object.RawJsonField(
      "artifacts",
      RenderFrontendCApiRunnerArtifactOwnershipArrayJson(ownership));
  object.End();
  return out.str();
}

}  // namespace

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
  out << "  \"paths\": " << RenderFrontendCApiRunnerPathsJson(public_result.paths)
      << ",\n";
  out << "  \"last_error\": \"" << EscapeJsonString(public_result.last_error)
      << "\",\n";
  out << "  \"result_error_message\": \""
      << EscapeJsonString(public_result.result_error_message) << "\",\n";
  out << "  \"c_api_ownership\": "
      << RenderFrontendCApiRunnerCOwnershipJson(public_result.c_api_ownership)
      << ",\n";
}
