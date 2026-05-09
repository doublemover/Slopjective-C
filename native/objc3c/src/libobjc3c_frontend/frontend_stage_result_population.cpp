#include "libobjc3c_frontend/frontend_stage_result_population.h"

#include "libobjc3c_frontend/objc3c_frontend_context_state.h"
#include "libobjc3c_frontend/objc3c_frontend_diagnostics.h"

namespace objc3c::frontend {

void ApplyFrontendResultStatusToContext(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product) {
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
}

void PopulateFrontendStageSummaries(
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan,
    bool sema_attempted,
    bool lower_attempted,
    const std::vector<std::string> &emit_diagnostics) {
  const bool emit_attempted = lower_attempted && artifact_plan.wants_emit_stage;
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
}

}  // namespace objc3c::frontend
