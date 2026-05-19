#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_TYPES_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_TYPES_H_

#include <stdint.h>

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

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_TYPES_H_
