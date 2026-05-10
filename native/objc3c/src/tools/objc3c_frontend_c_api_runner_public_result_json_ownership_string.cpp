#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership_string.h"

#include <sstream>

#include "io/json/json_writer.h"

using objc3::io::json::JsonObjectWriter;

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
