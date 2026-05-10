#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership.h"

#include <ostream>
#include <sstream>

#include "io/json/json_writer.h"

using objc3::io::json::JsonArrayWriter;
using objc3::io::json::JsonObjectWriter;

namespace {

std::string RenderFrontendCApiRunnerStringOwnershipJson(
    const FrontendCApiRunnerCStringOwnershipContract &contract) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.BoolField("present", contract.present);
  object.StringField("snapshot_owner", contract.snapshot_owner);
  object.StringField("accessor_name", contract.accessor_name);
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
  object.BoolField("path_snapshot_present", contract.path_snapshot_present);
  object.StringField("snapshot_owner", contract.snapshot_owner);
  object.StringField("presence_accessor", contract.presence_accessor);
  object.StringField("path_accessor", contract.path_accessor);
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
  object.StringField("publication_snapshot_owner",
                     ownership.publication_snapshot_owner);
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

void WriteFrontendCApiRunnerPublicResultOwnershipJsonRows(
    std::ostream &out,
    const FrontendCApiRunnerPublicResultView &public_result) {
  out << "  \"c_api_ownership\": "
      << RenderFrontendCApiRunnerCOwnershipJson(public_result.c_api_ownership)
      << ",\n";
}
