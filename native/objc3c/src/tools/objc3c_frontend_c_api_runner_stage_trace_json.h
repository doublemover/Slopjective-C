#pragma once

#include <ostream>
#include <string>

#include "libobjc3c_frontend/c_api.h"

void WriteFrontendCApiRunnerStageSummaryJson(
    std::ostream &out,
    const char *name,
    const objc3c_frontend_c_stage_summary_t &summary,
    bool trailing_comma);

std::string BuildFrontendCApiRunnerStageTraceJson(
    const objc3c_frontend_c_compile_result_t &result);
