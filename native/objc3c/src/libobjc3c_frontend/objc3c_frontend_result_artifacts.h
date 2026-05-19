#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_ARTIFACTS_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_ARTIFACTS_H_

#include <stdint.h>

#include "objc3c_frontend_artifact.h"
#include "objc3c_frontend_result_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Result-owned artifact accessors. Undefined artifact kinds, NULL results, and
 * absent payloads fail closed as NULL, {NULL, 0}, or 0.
 */
OBJC3C_FRONTEND_API const objc3c_frontend_string_t *
objc3c_frontend_result_artifact_path(
    const objc3c_frontend_compile_result_t *result,
    objc3c_frontend_artifact_kind_t artifact_kind);
OBJC3C_FRONTEND_API objc3c_frontend_string_view_t
objc3c_frontend_result_artifact_path_view(
    const objc3c_frontend_compile_result_t *result,
    objc3c_frontend_artifact_kind_t artifact_kind);
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_result_has_artifact(
    const objc3c_frontend_compile_result_t *result,
    objc3c_frontend_artifact_kind_t artifact_kind);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_ARTIFACTS_H_
