#pragma once

/*
 * Internal C++ owner for frontend C API conformance/report artifact
 * publication. This keeps report sidecars out of the public entrypoint body.
 */
#include "libobjc3c_frontend/objc3_cli_frontend.h"
#include "libobjc3c_frontend/objc3c_frontend_artifact_plan.h"
#include "libobjc3c_frontend/objc3c_frontend_context.h"
#include "libobjc3c_frontend/objc3c_frontend_result.h"

namespace objc3c::frontend {

bool PublishFrontendConformanceReportArtifacts(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan);

}  // namespace objc3c::frontend
