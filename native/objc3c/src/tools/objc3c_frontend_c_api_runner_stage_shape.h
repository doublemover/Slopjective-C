#pragma once

#include "libobjc3c_frontend/c_api.h"

bool FrontendCApiStageSummaryShapeReady(
    const objc3c_frontend_c_stage_summary_t &summary,
    objc3c_frontend_c_stage_id_t expected_stage);
bool FrontendCApiStageReportShapeReady(
    const objc3c_frontend_c_compile_result_t &result);
