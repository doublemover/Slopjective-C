#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership_aggregate_rows.h"

#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership_artifact_array.h"

void WriteFrontendCApiRunnerCOwnershipArtifactSummaryRows(
    objc3::io::json::JsonObjectWriter &object,
    const FrontendCApiRunnerCOwnershipView &ownership) {
  object.RawJsonField(
      "artifacts",
      RenderFrontendCApiRunnerArtifactOwnershipArrayJson(ownership));
}
