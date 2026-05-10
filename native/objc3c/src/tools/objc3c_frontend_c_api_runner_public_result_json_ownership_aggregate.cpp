#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership_aggregate.h"

#include <sstream>

#include "io/json/json_writer.h"
#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership_artifact_array.h"
#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership_string.h"

using objc3::io::json::JsonObjectWriter;

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
