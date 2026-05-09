#include "libobjc3c_frontend/frontend_stage_finalization.h"

#include "libobjc3c_frontend/frontend_stage_result_population.h"
#include "libobjc3c_frontend/objc3c_frontend_context_state.h"

namespace objc3c::frontend {

objc3c_frontend_status_t FinalizeFrontendCompileResult(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan,
    bool sema_attempted,
    bool lower_attempted,
    const std::vector<std::string> &emit_diagnostics) {
  ApplyFrontendResultStatusToContext(context, result, product);
  PopulateFrontendStageSummaries(result, product, artifact_plan, sema_attempted,
                                 lower_attempted, emit_diagnostics);

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
