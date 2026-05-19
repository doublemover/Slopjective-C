#pragma once

/*
 * Internal C++ owner for final status text, stage summaries, semantic skip
 * state, and result-owned payload attachment.
 */
#include <vector>
#include <string>

#include "libobjc3c_frontend/objc3_cli_frontend.h"
#include "libobjc3c_frontend/objc3c_frontend_artifact_plan.h"
#include "libobjc3c_frontend/objc3c_frontend_context.h"
#include "libobjc3c_frontend/objc3c_frontend_result.h"

namespace objc3c::frontend {

objc3c_frontend_status_t FinalizeFrontendCompileResult(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan,
    bool sema_attempted,
    bool lower_attempted,
    const std::vector<std::string> &emit_diagnostics);

}  // namespace objc3c::frontend
