#pragma once

/*
 * Internal C++ diagnostic serialization and stage-summary builder. Public
 * callers observe the value summaries and diagnostics artifact path only.
 */
#include <string>
#include <vector>

#include "libobjc3c_frontend/objc3c_frontend_diagnostic.h"

namespace objc3c::frontend {

std::string BuildFrontendDiagnosticsJson(
    const std::vector<std::string> &diagnostics);

objc3c_frontend_stage_summary_t BuildFrontendStageSummary(
    objc3c_frontend_stage_id_t stage_id,
    bool attempted,
    bool skipped,
    const std::vector<std::string> &diagnostics);

}  // namespace objc3c::frontend
