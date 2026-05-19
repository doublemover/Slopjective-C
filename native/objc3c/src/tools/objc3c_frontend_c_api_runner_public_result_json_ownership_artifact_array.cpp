#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership_artifact_array.h"

#include <sstream>

#include "io/json/json_writer.h"
#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership_artifact.h"

using objc3::io::json::JsonArrayWriter;

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
