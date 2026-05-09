#include "libobjc3c_frontend/objc3c_frontend_basic_artifacts.h"

#include "libobjc3c_frontend/objc3c_frontend_artifact_publication.h"
#include "libobjc3c_frontend/objc3c_frontend_core_artifacts.h"
#include "libobjc3c_frontend/objc3c_frontend_runtime_artifacts.h"

namespace objc3c::frontend {

bool PublishFrontendBasicArtifacts(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan,
    bool emit_manifest) {
  if (!PublishFrontendDiagnosticsArtifact(context, result, product,
                                          artifact_plan)) {
    return false;
  }

  SetFrontendDiagnosticOrOkStatus(result, product);

  if (!PublishFrontendManifestArtifact(context, result, product, artifact_plan,
                                       emit_manifest)) {
    return false;
  }

  if (!PublishFrontendRuntimeMetadataArtifact(context, result, product,
                                             artifact_plan)) {
    return false;
  }

  if (!PublishFrontendErrorBridgeReplayArtifact(context, result, product,
                                                artifact_plan)) {
    return false;
  }

  if (!PublishFrontendRuntimeAwareImportArtifact(context, result, product,
                                                artifact_plan)) {
    return false;
  }

  return PublishFrontendInteropBridgeArtifacts(context, result, product,
                                              artifact_plan);
}

}  // namespace objc3c::frontend
