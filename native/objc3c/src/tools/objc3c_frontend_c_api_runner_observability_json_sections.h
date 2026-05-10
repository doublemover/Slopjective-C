#pragma once

#include <iosfwd>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_observability_json_context.h"

void WriteFrontendCApiRunnerObservabilitySections(
    std::ostream &out,
    objc3c_frontend_c_status_t status,
    const FrontendCApiRunnerObservabilityContext &context);
