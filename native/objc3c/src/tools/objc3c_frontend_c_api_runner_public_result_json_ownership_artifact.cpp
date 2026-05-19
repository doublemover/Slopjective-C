#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership_artifact.h"

#include <sstream>

#include "io/json/json_writer.h"

using objc3::io::json::JsonObjectWriter;

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
  object.StringField("payload_owner", contract.payload_owner);
  object.StringField("accessor_view", contract.accessor_view);
  object.StringField("release_function", contract.release_function);
  object.StringField("null_contract", contract.null_contract);
  object.End();
  return out.str();
}
