#pragma once

/*
 * Internal C++ owner for runtime-aware artifact publication helpers. The public
 * ABI exposes only artifact selectors and result-owned paths.
 */
#include "libobjc3c_frontend/objc3_cli_frontend.h"
#include "libobjc3c_frontend/objc3c_frontend_artifact_plan.h"
#include "libobjc3c_frontend/objc3c_frontend_context.h"
#include "libobjc3c_frontend/objc3c_frontend_result.h"

namespace objc3c::frontend {

bool PublishFrontendErrorBridgeReplayArtifact(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan);

bool PublishFrontendRuntimeAwareImportArtifact(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan);

bool PublishFrontendInteropBridgeArtifacts(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan);

}  // namespace objc3c::frontend
