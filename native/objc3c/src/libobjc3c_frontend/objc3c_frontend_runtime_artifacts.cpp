#include "libobjc3c_frontend/objc3c_frontend_runtime_artifacts.h"

#include "artifacts/objc3_frontend_artifacts.h"
#include "io/objc3_artifact_paths.h"
#include "libobjc3c_frontend/objc3c_frontend_artifact_publication.h"
#include "pipeline/results/runtime_import_evidence_record.h"

namespace objc3c::frontend {

namespace {

bool FrontendPublicationIsOk(const objc3c_frontend_compile_result_t *result) {
  return result != nullptr && result->status == OBJC3C_FRONTEND_STATUS_OK;
}

}  // namespace

bool PublishFrontendErrorBridgeReplayArtifact(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan) {
  if (!FrontendPublicationIsOk(result) || !artifact_plan.has_out_dir ||
      product.artifact_bundle.error_handling_result_bridge_artifact_replay_json
          .empty()) {
    return true;
  }

  return WriteFrontendTextArtifactOrError(
      context, result,
      BuildErrorHandlingResultBridgeArtifactReplayPath(
          artifact_plan.out_dir, artifact_plan.emit_prefix),
      product.artifact_bundle.error_handling_result_bridge_artifact_replay_json);
}

bool PublishFrontendRuntimeAwareImportArtifact(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan) {
  if (!FrontendPublicationIsOk(result) || !artifact_plan.has_out_dir ||
      product.artifact_bundle.runtime_aware_import_module_artifact_json
          .empty()) {
    return true;
  }

  if (!IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
          product.artifact_bundle
              .runtime_aware_import_module_frontend_closure_summary)) {
    SetFrontendPublicationError(
        context, result,
        "runtime-aware import/module frontend closure not ready");
    return false;
  }

  return WriteFrontendTextArtifactOrError(
      context, result,
      BuildRuntimeAwareImportModuleArtifactPath(artifact_plan.out_dir,
                                               artifact_plan.emit_prefix),
      product.artifact_bundle.runtime_aware_import_module_artifact_json);
}

bool PublishFrontendInteropBridgeArtifacts(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan) {
  if (!FrontendPublicationIsOk(result) || !artifact_plan.has_out_dir) {
    return true;
  }

  if (!product.artifact_bundle.interop_bridge_header_artifact_text.empty() &&
      !WriteFrontendTextArtifactOrError(
          context, result,
          BuildInteropBridgeHeaderArtifactPath(artifact_plan.out_dir,
                                              artifact_plan.emit_prefix),
          product.artifact_bundle.interop_bridge_header_artifact_text)) {
    return false;
  }
  if (!product.artifact_bundle.interop_bridge_module_artifact_text.empty() &&
      !WriteFrontendTextArtifactOrError(
          context, result,
          BuildInteropBridgeModuleArtifactPath(artifact_plan.out_dir,
                                              artifact_plan.emit_prefix),
          product.artifact_bundle.interop_bridge_module_artifact_text)) {
    return false;
  }
  if (!product.artifact_bundle.interop_bridge_artifact_json.empty() &&
      !WriteFrontendTextArtifactOrError(
          context, result,
          BuildInteropBridgeArtifactPath(artifact_plan.out_dir,
                                        artifact_plan.emit_prefix),
          product.artifact_bundle.interop_bridge_artifact_json)) {
    return false;
  }

  return true;
}

}  // namespace objc3c::frontend
