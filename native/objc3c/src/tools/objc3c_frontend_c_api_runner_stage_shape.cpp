#include "tools/objc3c_frontend_c_api_runner_stage_shape.h"

bool FrontendCApiStageSummaryShapeReady(
    const objc3c_frontend_c_stage_summary_t &summary,
    objc3c_frontend_c_stage_id_t expected_stage) {
  return objc3c_frontend_c_stage_summary_is_well_formed(&summary,
                                                        expected_stage) != 0u;
}

bool FrontendCApiStageReportShapeReady(
    const objc3c_frontend_c_compile_result_t &result) {
  return FrontendCApiStageSummaryShapeReady(result.lex,
                                           OBJC3C_FRONTEND_STAGE_LEX) &&
         FrontendCApiStageSummaryShapeReady(result.parse,
                                           OBJC3C_FRONTEND_STAGE_PARSE) &&
         FrontendCApiStageSummaryShapeReady(result.sema,
                                           OBJC3C_FRONTEND_STAGE_SEMA) &&
         FrontendCApiStageSummaryShapeReady(result.lower,
                                           OBJC3C_FRONTEND_STAGE_LOWER) &&
         FrontendCApiStageSummaryShapeReady(result.emit,
                                           OBJC3C_FRONTEND_STAGE_EMIT);
}
