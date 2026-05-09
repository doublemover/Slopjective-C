#include "libobjc3c_frontend/frontend_stage_finalization.h"

#include "libobjc3c_frontend/objc3c_frontend_context_state.h"
#include "libobjc3c_frontend/objc3c_frontend_diagnostics.h"

namespace objc3c::frontend {

objc3c_frontend_status_t FinalizeFrontendCompileResult(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan,
    bool sema_attempted,
    bool lower_attempted,
    const std::vector<std::string> &emit_diagnostics) {
  if (result->status == OBJC3C_FRONTEND_STATUS_DIAGNOSTICS) {
    if (!product.artifact_bundle.diagnostics.empty()) {
      SetFrontendContextError(context,
                              product.artifact_bundle.diagnostics.front().c_str());
    } else {
      SetFrontendContextError(context, "compilation reported diagnostics.");
    }
  } else if (result->status == OBJC3C_FRONTEND_STATUS_OK) {
    SetFrontendContextError(context, "");
  }

  const bool wants_emit_stage = artifact_plan.wants_emit_stage;
  const bool emit_attempted = lower_attempted && wants_emit_stage;
  const bool emit_skipped = !emit_attempted;
  result->semantic_skipped =
      product.pipeline_result.integration_surface.built ? 0u : 1u;

  result->lex = BuildFrontendStageSummary(
      OBJC3C_FRONTEND_STAGE_LEX, true, false,
      product.pipeline_result.stage_diagnostics.lexer);
  result->parse = BuildFrontendStageSummary(
      OBJC3C_FRONTEND_STAGE_PARSE, true, false,
      product.pipeline_result.stage_diagnostics.parser);
  result->sema = BuildFrontendStageSummary(
      OBJC3C_FRONTEND_STAGE_SEMA, sema_attempted, !sema_attempted,
      product.pipeline_result.stage_diagnostics.semantic);
  result->lower = BuildFrontendStageSummary(
      OBJC3C_FRONTEND_STAGE_LOWER, lower_attempted, !lower_attempted, {});
  result->emit = BuildFrontendStageSummary(
      OBJC3C_FRONTEND_STAGE_EMIT, emit_attempted, emit_skipped,
      emit_diagnostics);

  std::string result_ownership_error;
  if (!PopulateCompileResultFromFrontendContext(context, result,
                                                result_ownership_error)) {
    result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
    result->process_exit_code = 2;
    result->success = 0;
    SetFrontendContextError(context, result_ownership_error.c_str());
  }
  return result->status;
}

}  // namespace objc3c::frontend
