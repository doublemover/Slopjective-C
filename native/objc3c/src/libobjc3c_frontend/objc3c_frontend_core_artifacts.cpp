#include "libobjc3c_frontend/objc3c_frontend_core_artifacts.h"

#include "artifacts/objc3_frontend_artifacts.h"
#include "io/objc3_artifact_paths.h"
#include "libobjc3c_frontend/objc3c_frontend_artifact_publication.h"
#include "libobjc3c_frontend/objc3c_frontend_context_state.h"
#include "libobjc3c_frontend/objc3c_frontend_diagnostics.h"

namespace objc3c::frontend {

bool PublishFrontendDiagnosticsArtifact(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan) {
  if (!artifact_plan.has_out_dir) {
    return true;
  }

  const std::filesystem::path diagnostics_out =
      BuildFrontendDiagnosticsOutputPath(artifact_plan);
  if (!WriteFrontendTextArtifactOrError(
          context, result, diagnostics_out,
          BuildFrontendDiagnosticsJson(product.artifact_bundle.diagnostics))) {
    return false;
  }
  SetFrontendContextDiagnosticsPath(context, diagnostics_out.generic_string());
  return true;
}

bool PublishFrontendManifestArtifact(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan,
    bool emit_manifest) {
  if (!emit_manifest || !artifact_plan.has_out_dir ||
      product.artifact_bundle.manifest_json.empty()) {
    return true;
  }

  const std::filesystem::path manifest_out =
      BuildFrontendManifestOutputPath(artifact_plan);
  if (!WriteFrontendTextArtifactOrError(
          context, result, manifest_out,
          product.artifact_bundle.manifest_json)) {
    return false;
  }
  SetFrontendContextManifestPath(context, manifest_out.generic_string());
  return true;
}

bool PublishFrontendRuntimeMetadataArtifact(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan) {
  if (!artifact_plan.has_out_dir ||
      product.artifact_bundle.runtime_metadata_binary.empty()) {
    return true;
  }

  const std::filesystem::path runtime_metadata_out =
      BuildRuntimeMetadataBinaryArtifactPath(artifact_plan.out_dir,
                                            artifact_plan.emit_prefix);
  if (!WriteFrontendBinaryArtifactOrError(
          context, result, runtime_metadata_out,
          product.artifact_bundle.runtime_metadata_binary)) {
    return false;
  }
  SetFrontendContextRuntimeMetadataPath(context,
                                        runtime_metadata_out.generic_string());
  return true;
}

}  // namespace objc3c::frontend
