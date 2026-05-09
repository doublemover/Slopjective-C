#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_H_

#include <stdint.h>

#include "objc3c_frontend_artifact.h"
#include "objc3c_frontend_diagnostic.h"
#include "objc3c_frontend_string.h"

/*
 * Top-level compile status values returned by compile entrypoints. Status
 * values classify the completed call; detailed parser/sema/lower/emit
 * diagnostics live in the result-owned diagnostics artifact when produced.
 */
typedef enum objc3c_frontend_status {
  OBJC3C_FRONTEND_STATUS_OK = 0,
  OBJC3C_FRONTEND_STATUS_DIAGNOSTICS = 1,
  OBJC3C_FRONTEND_STATUS_USAGE_ERROR = 2,
  OBJC3C_FRONTEND_STATUS_EMIT_ERROR = 3,
  OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR = 4
} objc3c_frontend_status_t;

/*
 * Caller-owned compile output storage populated by compile entrypoints. The
 * result storage must be zero-initialized before first use and must be released
 * with objc3c_frontend_result_destroy() before reuse.
 *
 * Non-NULL string members are owned by this result and released only by
 * objc3c_frontend_result_destroy(); callers must not release them directly.
 * NULL string members mean the payload was not produced for that invocation.
 * Stage summaries are value snapshots; they do not borrow transient diagnostic
 * storage from the pipeline.
 */
typedef struct objc3c_frontend_compile_result {
  objc3c_frontend_status_t status;
  int32_t process_exit_code;
  uint8_t success;
  uint8_t semantic_skipped;
  uint16_t reserved;
  objc3c_frontend_stage_summary_t lex;
  objc3c_frontend_stage_summary_t parse;
  objc3c_frontend_stage_summary_t sema;
  objc3c_frontend_stage_summary_t lower;
  objc3c_frontend_stage_summary_t emit;
  objc3c_frontend_string_t *error_message;
  objc3c_frontend_string_t *diagnostics_path;
  objc3c_frontend_string_t *manifest_path;
  objc3c_frontend_string_t *runtime_metadata_path;
  objc3c_frontend_string_t *ir_path;
  objc3c_frontend_string_t *object_path;
} objc3c_frontend_compile_result_t;

#ifdef __cplusplus
extern "C" {
#endif

OBJC3C_FRONTEND_API void objc3c_frontend_result_destroy(
    objc3c_frontend_compile_result_t *result);
/*
 * Returns a borrowed pointer to a result-owned artifact path string for a
 * produced artifact. Returns NULL when result is NULL, the artifact was not
 * produced, or artifact_kind is not a defined objc3c_frontend_artifact_kind_t.
 * The pointer remains valid until objc3c_frontend_result_destroy(result).
 * The returned pointer is borrowed and must not be passed to
 * objc3c_frontend_string_release().
 */
OBJC3C_FRONTEND_API const objc3c_frontend_string_t *
objc3c_frontend_result_artifact_path(
    const objc3c_frontend_compile_result_t *result,
    objc3c_frontend_artifact_kind_t artifact_kind);
/*
 * Value-view form of objc3c_frontend_result_artifact_path(). The returned view
 * is borrowed and remains valid only until objc3c_frontend_result_destroy().
 */
OBJC3C_FRONTEND_API objc3c_frontend_string_view_t
objc3c_frontend_result_artifact_path_view(
    const objc3c_frontend_compile_result_t *result,
    objc3c_frontend_artifact_kind_t artifact_kind);
/* Returns non-zero only when the selected result-owned artifact path exists. */
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_result_has_artifact(
    const objc3c_frontend_compile_result_t *result,
    objc3c_frontend_artifact_kind_t artifact_kind);
/*
 * Returns a borrowed pointer to the result-owned error string. Returns NULL
 * when result is NULL or no error payload was produced. The pointer remains
 * valid until objc3c_frontend_result_destroy(result).
 * The returned pointer is borrowed and must not be passed to
 * objc3c_frontend_string_release().
 */
OBJC3C_FRONTEND_API const objc3c_frontend_string_t *
objc3c_frontend_result_error_message(
    const objc3c_frontend_compile_result_t *result);
/*
 * Value-view form of objc3c_frontend_result_error_message(). The returned view
 * is borrowed and remains valid only until objc3c_frontend_result_destroy().
 */
OBJC3C_FRONTEND_API objc3c_frontend_string_view_t
objc3c_frontend_result_error_message_view(
    const objc3c_frontend_compile_result_t *result);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_H_
