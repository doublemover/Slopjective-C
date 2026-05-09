#ifndef OBJC3C_LIBOBJC3C_FRONTEND_C_API_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_C_API_H_

#include <stddef.h>
#include <stdint.h>

#include "objc3c_frontend.h"

#define OBJC3C_FRONTEND_C_API_ABI_VERSION 1u

#ifdef __cplusplus
extern "C" {
#endif

/*
 * C ABI wrapper for non-C++ embedding environments. It preserves the
 * underlying objc3c_frontend ABI data structures and forwards to the primary
 * libobjc3c_frontend entrypoints. Result/string ownership is explicit on this
 * wrapper surface: compile_result storage is caller-owned, result payload
 * strings are released only by objc3c_frontend_c_result_destroy(), and borrowed
 * option strings/paths must remain valid for the duration of the call.
 */
typedef objc3c_frontend_context_t objc3c_frontend_c_context_t;
typedef objc3c_frontend_stage_id_t objc3c_frontend_c_stage_id_t;
typedef objc3c_frontend_status_t objc3c_frontend_c_status_t;
typedef objc3c_frontend_diagnostic_severity_t objc3c_frontend_c_diagnostic_severity_t;
typedef objc3c_frontend_ir_object_backend_t objc3c_frontend_c_ir_object_backend_t;
typedef objc3c_frontend_artifact_kind_t objc3c_frontend_c_artifact_kind_t;
typedef objc3c_frontend_string_t objc3c_frontend_c_string_t;
typedef objc3c_frontend_string_view_t objc3c_frontend_c_string_view_t;
typedef objc3c_frontend_stage_summary_t objc3c_frontend_c_stage_summary_t;
typedef objc3c_frontend_compile_options_t objc3c_frontend_c_compile_options_t;
typedef objc3c_frontend_compile_result_t objc3c_frontend_c_compile_result_t;
typedef objc3c_frontend_version_t objc3c_frontend_c_version_t;

OBJC3C_FRONTEND_API uint32_t objc3c_frontend_c_api_abi_version(void);

OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_is_abi_compatible(
    uint32_t requested_abi_version);
OBJC3C_FRONTEND_API uint32_t objc3c_frontend_c_abi_version(void);
OBJC3C_FRONTEND_API objc3c_frontend_c_version_t objc3c_frontend_c_version(void);
/* Returns static read-only version storage; callers must not release it. */
OBJC3C_FRONTEND_API const char *objc3c_frontend_c_version_string(void);

OBJC3C_FRONTEND_API objc3c_frontend_c_context_t *objc3c_frontend_c_context_create(void);
OBJC3C_FRONTEND_API void objc3c_frontend_c_context_destroy(objc3c_frontend_c_context_t *context);

OBJC3C_FRONTEND_API objc3c_frontend_c_status_t objc3c_frontend_c_compile_file(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_compile_result_t *result);

OBJC3C_FRONTEND_API objc3c_frontend_c_status_t objc3c_frontend_c_compile_source(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_compile_result_t *result);

/*
 * Null-safe ownership/accessor surface for C-only embedders.
 * - result_destroy releases only result-owned payload strings, then zeros the result.
 * - result_* accessors return borrowed pointers/views valid until result_destroy.
 * - string_release is for standalone owned strings only; do not pass result-owned
 *   strings returned by result_error_message/result_artifact_path.
 * - undefined artifact kinds and NULL inputs fail closed as NULL/empty/0.
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
OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t objc3c_frontend_c_string_view(
    const objc3c_frontend_c_string_t *string);
OBJC3C_FRONTEND_API void objc3c_frontend_c_string_release(
    objc3c_frontend_c_string_t *string);

OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_stage_summary_is_well_formed(
    const objc3c_frontend_c_stage_summary_t *summary,
    objc3c_frontend_c_stage_id_t expected_stage);
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_stage_summary_has_diagnostics(
    const objc3c_frontend_c_stage_summary_t *summary);
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_stage_summary_has_errors(
    const objc3c_frontend_c_stage_summary_t *summary);

OBJC3C_FRONTEND_API size_t objc3c_frontend_c_copy_last_error(
    const objc3c_frontend_c_context_t *context,
    char *buffer,
    size_t buffer_size);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_C_API_H_
