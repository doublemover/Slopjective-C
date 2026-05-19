#ifndef OBJC3C_LIBOBJC3C_FRONTEND_PUBLIC_C_API_OWNED_RESULT_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_PUBLIC_C_API_OWNED_RESULT_H_

#include <stdint.h>

#include "libobjc3c_frontend/c_api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3c_frontend_c_result objc3c_frontend_c_result_t;

/*
 * Owned result handle surface for C-only embedders. Successful calls allocate a
 * result handle and return it through out_result; failing compile calls still
 * return a handle when result storage could be allocated so callers can inspect
 * deterministic error, diagnostic, stage, and artifact payloads. The handle and
 * every result-owned string visible through its accessors are released only by
 * objc3c_frontend_c_owned_result_destroy().
 */
OBJC3C_FRONTEND_API objc3c_frontend_c_status_t
objc3c_frontend_c_compile_file_owned(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_result_t **out_result);
OBJC3C_FRONTEND_API objc3c_frontend_c_status_t
objc3c_frontend_c_compile_source_owned(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_result_t **out_result);

OBJC3C_FRONTEND_API void objc3c_frontend_c_owned_result_destroy(
    objc3c_frontend_c_result_t *result);

OBJC3C_FRONTEND_API objc3c_frontend_c_status_t
objc3c_frontend_c_owned_result_status(
    const objc3c_frontend_c_result_t *result);
OBJC3C_FRONTEND_API int32_t objc3c_frontend_c_owned_result_process_exit_code(
    const objc3c_frontend_c_result_t *result);
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_owned_result_success(
    const objc3c_frontend_c_result_t *result);
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_owned_result_semantic_skipped(
    const objc3c_frontend_c_result_t *result);

OBJC3C_FRONTEND_API objc3c_frontend_c_stage_summary_t
objc3c_frontend_c_owned_result_stage_summary(
    const objc3c_frontend_c_result_t *result,
    objc3c_frontend_c_stage_id_t stage);

OBJC3C_FRONTEND_API const objc3c_frontend_c_string_t *
objc3c_frontend_c_owned_result_error_message(
    const objc3c_frontend_c_result_t *result);
OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_owned_result_error_message_view(
    const objc3c_frontend_c_result_t *result);

OBJC3C_FRONTEND_API const objc3c_frontend_c_string_t *
objc3c_frontend_c_owned_result_artifact_path(
    const objc3c_frontend_c_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind);
OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_owned_result_artifact_path_view(
    const objc3c_frontend_c_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind);
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_owned_result_has_artifact(
    const objc3c_frontend_c_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_PUBLIC_C_API_OWNED_RESULT_H_
