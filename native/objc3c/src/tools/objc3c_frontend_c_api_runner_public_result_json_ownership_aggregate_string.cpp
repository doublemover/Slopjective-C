#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership_aggregate_rows.h"

#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership_string.h"

void WriteFrontendCApiRunnerCOwnershipStringNullRows(
    objc3::io::json::JsonObjectWriter &object,
    const FrontendCApiRunnerCOwnershipView &ownership) {
  object.StringField("standalone_string_release_function",
                     ownership.standalone_string_release_function);
  object.RawJsonField(
      "error_message",
      RenderFrontendCApiRunnerStringOwnershipJson(ownership.error_message));
}
