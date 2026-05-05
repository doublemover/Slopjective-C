#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_H_

#include <stdint.h>

#include "objc3c_frontend_artifact.h"
#include "objc3c_frontend_diagnostic.h"

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
 * Path pointers may be NULL when artifacts are unavailable or not emitted.
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
  const char *diagnostics_path;
  const char *manifest_path;
  const char *ir_path;
  const char *object_path;
} objc3c_frontend_compile_result_t;

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_H_
