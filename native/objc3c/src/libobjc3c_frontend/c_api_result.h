#ifndef OBJC3C_LIBOBJC3C_FRONTEND_C_API_RESULT_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_C_API_RESULT_H_

#include <stdint.h>

#include "c_api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Ownership/accessor surface for C-only embedders.
 * - result_destroy releases only result-owned payload strings, then zeros the
 *   result. Passing NULL is a no-op because there is no owner to release.
 * - result_* accessors return borrowed pointers/views valid until
 *   result_destroy.
 * - string_release is for standalone owned strings only; do not pass
 *   result-owned strings returned by result_error_message/result_artifact_path.
 * - undefined artifact kinds and NULL inputs fail closed as NULL/empty/0.
 * - NULL results, absent payloads, and undefined artifact kinds never
 *   manufacture fallback values: pointer accessors return NULL, views return
 *   {NULL, 0}, and boolean predicates return 0.
 */
OBJC3C_FRONTEND_API void objc3c_frontend_c_result_destroy(
    objc3c_frontend_c_compile_result_t *result);
OBJC3C_FRONTEND_API const objc3c_frontend_c_string_t *
objc3c_frontend_c_result_artifact_path(
    const objc3c_frontend_c_compile_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind);
OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_result_artifact_path_view(
    const objc3c_frontend_c_compile_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind);
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_result_has_artifact(
    const objc3c_frontend_c_compile_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind);
OBJC3C_FRONTEND_API const objc3c_frontend_c_string_t *
objc3c_frontend_c_result_error_message(
    const objc3c_frontend_c_compile_result_t *result);
OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_result_error_message_view(
    const objc3c_frontend_c_compile_result_t *result);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_C_API_RESULT_H_
