#include "libobjc3c_frontend/frontend_compile_pipeline.h"

#include "io/objc3_manifest_artifacts.h"
#include "libobjc3c_frontend/frontend_lowering_boundary.h"
#include "libobjc3c_frontend/objc3c_frontend_basic_artifacts.h"
#include "libobjc3c_frontend/objc3c_frontend_compile_contract.h"
#include "libobjc3c_frontend/objc3c_frontend_context_state.h"
#include "libobjc3c_frontend/objc3c_frontend_result_ownership.h"

namespace objc3c::frontend {

namespace {

void SetCompileRunError(objc3c_frontend_context_t *context,
                        objc3c_frontend_compile_result_t *result,
                        objc3c_frontend_status_t status,
                        int process_exit_code,
                        const std::string &message) {
  result->status = status;
  result->process_exit_code = process_exit_code;
  result->success = 0;
  SetFrontendContextError(context, message.c_str());
}

}  // namespace

bool PrepareFrontendCompileRun(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const std::filesystem::path &input_path,
    const std::string &source_text,
    const objc3c_frontend_compile_options_t &options,
    Objc3FrontendCompileRun &run) {
  ResetCompileResultForWrite(result);
  ClearFrontendContextResultPaths(context);

  Objc3FrontendOptions frontend_options = BuildFrontendPipelineOptions(options);
  std::string lowering_error;
  if (!TryNormalizeFrontendLoweringBoundary(frontend_options,
                                            lowering_error)) {
    SetCompileRunError(context, result, OBJC3C_FRONTEND_STATUS_USAGE_ERROR, 2,
                       lowering_error);
    return false;
  }

  run.product =
      CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options);
  run.sema_attempted =
      run.product.pipeline_result.stage_diagnostics.lexer.empty() &&
      run.product.pipeline_result.stage_diagnostics.parser.empty();
  run.lower_attempted =
      run.sema_attempted &&
      run.product.pipeline_result.stage_diagnostics.semantic.empty();
  run.emit_diagnostics =
      run.product.artifact_bundle.post_pipeline_diagnostics;
  run.artifact_plan = BuildFrontendArtifactOutputPlan(options, input_path);

  if (run.artifact_plan.has_out_dir) {
    std::string retired_claim_sidecar_error;
    if (!DiagnoseObjc3RetiredClaimSidecars(run.artifact_plan.out_dir,
                                           run.artifact_plan.emit_prefix,
                                           retired_claim_sidecar_error)) {
      SetCompileRunError(context, result, OBJC3C_FRONTEND_STATUS_USAGE_ERROR,
                         125, retired_claim_sidecar_error);
      return false;
    }
  }

  return PublishFrontendBasicArtifacts(context, result, run.product,
                                       run.artifact_plan,
                                       options.emit_manifest != 0);
}

}  // namespace objc3c::frontend
