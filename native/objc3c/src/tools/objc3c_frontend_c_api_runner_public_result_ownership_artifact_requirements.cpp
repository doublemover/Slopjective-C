#include "tools/objc3c_frontend_c_api_runner_public_result_ownership.h"

std::array<FrontendCApiRunnerCArtifactRequirement, 5>
BuildFrontendCApiRunnerCArtifactRequirements(
    const FrontendCApiRunnerOptions &options,
    objc3c_frontend_c_status_t status) {
  const bool ok_status = status == OBJC3C_FRONTEND_STATUS_OK;
  const bool pipeline_status =
      ok_status || status == OBJC3C_FRONTEND_STATUS_DIAGNOSTICS ||
      status == OBJC3C_FRONTEND_STATUS_EMIT_ERROR;
  return {{
      {OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS,
       "diagnostics",
       pipeline_status},
      {OBJC3C_FRONTEND_ARTIFACT_MANIFEST,
       "manifest",
       options.emit_manifest && pipeline_status},
      {OBJC3C_FRONTEND_ARTIFACT_IR,
       "ir",
       (options.emit_ir || options.emit_object) && ok_status},
      {OBJC3C_FRONTEND_ARTIFACT_OBJECT,
       "object",
       options.emit_object && ok_status},
      {OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA,
       "runtime_metadata_binary",
       false},
  }};
}
