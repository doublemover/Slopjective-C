#ifndef OBJC3C_LIBOBJC3C_FRONTEND_C_API_STAGE_SUMMARY_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_C_API_STAGE_SUMMARY_H_

#include <stdint.h>

#include "c_api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Stage-summary owner for C-only embedders. Predicates inspect only the
 * by-value public summary record and reject NULL, undefined stage ids, mutually
 * exclusive attempted/skipped states, and severity totals that drift from
 * diagnostics_total.
 */
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_stage_summary_is_well_formed(
    const objc3c_frontend_c_stage_summary_t *summary,
    objc3c_frontend_c_stage_id_t expected_stage);
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_stage_summary_has_diagnostics(
    const objc3c_frontend_c_stage_summary_t *summary);
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_stage_summary_has_errors(
    const objc3c_frontend_c_stage_summary_t *summary);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_C_API_STAGE_SUMMARY_H_
