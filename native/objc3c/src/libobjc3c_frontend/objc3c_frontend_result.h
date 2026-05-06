#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_H_

#include <stdint.h>

#include "objc3c_frontend_artifact.h"
#include "objc3c_frontend_diagnostic.h"
#include "objc3c_frontend_string.h"

/* Top-level compile status values returned by compile entrypoints. */
typedef enum objc3c_frontend_status {
  OBJC3C_FRONTEND_STATUS_OK = 0,
  OBJC3C_FRONTEND_STATUS_DIAGNOSTICS = 1,
  OBJC3C_FRONTEND_STATUS_USAGE_ERROR = 2,
  OBJC3C_FRONTEND_STATUS_EMIT_ERROR = 3,
  OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR = 4
} objc3c_frontend_status_t;

/*
 * Caller-owned compile output struct populated by compile entrypoints.
 * Non-NULL strings are owned by the result and released by
 * objc3c_frontend_result_destroy().
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
OBJC3C_FRONTEND_API const objc3c_frontend_string_t *
objc3c_frontend_result_artifact_path(
    const objc3c_frontend_compile_result_t *result,
    objc3c_frontend_artifact_kind_t artifact_kind);
OBJC3C_FRONTEND_API const objc3c_frontend_string_t *
objc3c_frontend_result_error_message(
    const objc3c_frontend_compile_result_t *result);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_H_
