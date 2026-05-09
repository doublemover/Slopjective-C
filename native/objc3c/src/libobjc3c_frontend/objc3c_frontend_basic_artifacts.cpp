#include "libobjc3c_frontend/objc3c_frontend_basic_artifacts.h"

#include "artifacts/objc3_frontend_artifacts.h"
#include "io/objc3_manifest_artifacts.h"
#include "libobjc3c_frontend/objc3c_frontend_context_state.h"
#include "libobjc3c_frontend/objc3c_frontend_diagnostics.h"
#include "libobjc3c_frontend/objc3c_frontend_file_output.h"

namespace objc3c::frontend {
namespace {

void SetInternalPublicationError(objc3c_frontend_context_t *context,
                                 objc3c_frontend_compile_result_t *result,
                                 const std::string &message) {
  result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
  result->process_exit_code = 2;
  result->success = 0;
  SetFrontendContextError(context, message.c_str());
}

void SetDiagnosticOrOkStatus(objc3c_frontend_compile_result_t *result,
                             const Objc3FrontendCompileProduct &product) {
  if (!product.artifact_bundle.diagnostics.empty()) {
    result->status = OBJC3C_FRONTEND_STATUS_DIAGNOSTICS;
    result->process_exit_code = 1;
    result->success = 0;
    return;
  }
  result->status = OBJC3C_FRONTEND_STATUS_OK;
  result->process_exit_code = 0;
  result->success = 1;
}

bool WriteOptionalTextArtifact(objc3c_frontend_context_t *context,
                               objc3c_frontend_compile_result_t *result,
                               const std::filesystem::path &path,
                               const std::string &contents) {
  std::string io_error;
  if (WriteFrontendTextFile(path, contents, io_error)) {
    return true;
  }
  SetInternalPublicationError(context, result, io_error);
  return false;
}

}  // namespace

bool PublishFrontendBasicArtifacts(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan,
    bool emit_manifest) {
  const std::filesystem::path &out_dir = artifact_plan.out_dir;
  const bool has_out_dir = artifact_plan.has_out_dir;
  const std::string &emit_prefix = artifact_plan.emit_prefix;

  if (has_out_dir) {
    const std::filesystem::path diagnostics_out =
        BuildFrontendDiagnosticsOutputPath(artifact_plan);
    std::string io_error;
    if (!WriteFrontendTextFile(
            diagnostics_out,
            BuildFrontendDiagnosticsJson(product.artifact_bundle.diagnostics),
            io_error)) {
      SetInternalPublicationError(context, result, io_error);
      return false;
    }
    SetFrontendContextDiagnosticsPath(context, diagnostics_out.generic_string());
  }

  SetDiagnosticOrOkStatus(result, product);

  if (emit_manifest && has_out_dir &&
      !product.artifact_bundle.manifest_json.empty()) {
    const std::filesystem::path manifest_out =
        BuildFrontendManifestOutputPath(artifact_plan);
    if (WriteOptionalTextArtifact(context, result, manifest_out,
                                  product.artifact_bundle.manifest_json)) {
      SetFrontendContextManifestPath(context, manifest_out.generic_string());
    }
  }

  if (has_out_dir && !product.artifact_bundle.runtime_metadata_binary.empty()) {
    const std::filesystem::path runtime_metadata_out =
        BuildRuntimeMetadataBinaryArtifactPath(out_dir, emit_prefix);
    std::string io_error;
    if (!WriteFrontendBinaryFile(runtime_metadata_out,
                                 product.artifact_bundle.runtime_metadata_binary,
                                 io_error)) {
      SetInternalPublicationError(context, result, io_error);
    } else {
      SetFrontendContextRuntimeMetadataPath(context,
                                            runtime_metadata_out.generic_string());
    }
  }

  if (result->status == OBJC3C_FRONTEND_STATUS_OK && has_out_dir &&
      !product.artifact_bundle.error_handling_result_bridge_artifact_replay_json
           .empty()) {
    const std::filesystem::path replay_out =
        BuildErrorHandlingResultBridgeArtifactReplayPath(out_dir, emit_prefix);
    WriteOptionalTextArtifact(
        context, result, replay_out,
        product.artifact_bundle.error_handling_result_bridge_artifact_replay_json);
  }

  if (result->status == OBJC3C_FRONTEND_STATUS_OK && has_out_dir) {
    const bool has_runtime_import_artifact =
        !product.artifact_bundle.runtime_aware_import_module_artifact_json
             .empty();
    if (has_runtime_import_artifact &&
        !IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
            product.artifact_bundle
                .runtime_aware_import_module_frontend_closure_summary)) {
      SetInternalPublicationError(
          context, result,
          "runtime-aware import/module frontend closure not ready");
    } else if (has_runtime_import_artifact) {
      WriteOptionalTextArtifact(
          context, result,
          BuildRuntimeAwareImportModuleArtifactPath(out_dir, emit_prefix),
          product.artifact_bundle.runtime_aware_import_module_artifact_json);
    }
  }

  if (result->status == OBJC3C_FRONTEND_STATUS_OK && has_out_dir &&
      !product.artifact_bundle.interop_bridge_header_artifact_text.empty()) {
    WriteOptionalTextArtifact(
        context, result, BuildInteropBridgeHeaderArtifactPath(out_dir, emit_prefix),
        product.artifact_bundle.interop_bridge_header_artifact_text);
  }
  if (result->status == OBJC3C_FRONTEND_STATUS_OK && has_out_dir &&
      !product.artifact_bundle.interop_bridge_module_artifact_text.empty()) {
    WriteOptionalTextArtifact(
        context, result, BuildInteropBridgeModuleArtifactPath(out_dir, emit_prefix),
        product.artifact_bundle.interop_bridge_module_artifact_text);
  }
  if (result->status == OBJC3C_FRONTEND_STATUS_OK && has_out_dir &&
      !product.artifact_bundle.interop_bridge_artifact_json.empty()) {
    WriteOptionalTextArtifact(
        context, result, BuildInteropBridgeArtifactPath(out_dir, emit_prefix),
        product.artifact_bundle.interop_bridge_artifact_json);
  }

  return true;
}

}  // namespace objc3c::frontend
