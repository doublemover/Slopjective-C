#include "libobjc3c_frontend/c_api.h"

extern "C" OBJC3C_FRONTEND_API uint8_t
objc3c_frontend_c_stage_summary_is_well_formed(
    const objc3c_frontend_c_stage_summary_t *summary,
    objc3c_frontend_c_stage_id_t expected_stage) {
  return objc3c_frontend_stage_summary_is_well_formed(summary, expected_stage);
}

extern "C" OBJC3C_FRONTEND_API uint8_t
objc3c_frontend_c_stage_summary_has_diagnostics(
    const objc3c_frontend_c_stage_summary_t *summary) {
  return objc3c_frontend_stage_summary_has_diagnostics(summary);
}

extern "C" OBJC3C_FRONTEND_API uint8_t
objc3c_frontend_c_stage_summary_has_errors(
    const objc3c_frontend_c_stage_summary_t *summary) {
  return objc3c_frontend_stage_summary_has_errors(summary);
}
