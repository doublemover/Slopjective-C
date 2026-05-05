#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_DIAGNOSTIC_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_DIAGNOSTIC_H_

#include <stdint.h>

/* Deterministic stage identifiers for per-stage summaries in compile results. */
typedef enum objc3c_frontend_stage_id {
  OBJC3C_FRONTEND_STAGE_LEX = 0,
  OBJC3C_FRONTEND_STAGE_PARSE = 1,
  OBJC3C_FRONTEND_STAGE_SEMA = 2,
  OBJC3C_FRONTEND_STAGE_LOWER = 3,
  OBJC3C_FRONTEND_STAGE_EMIT = 4
} objc3c_frontend_stage_id_t;

/* Canonical diagnostic severities used in stage and output diagnostics metadata. */
typedef enum objc3c_frontend_diagnostic_severity {
  OBJC3C_FRONTEND_DIAG_NOTE = 0,
  OBJC3C_FRONTEND_DIAG_WARNING = 1,
  OBJC3C_FRONTEND_DIAG_ERROR = 2,
  OBJC3C_FRONTEND_DIAG_FATAL = 3
} objc3c_frontend_diagnostic_severity_t;

/* Per-stage execution summary written to objc3c_frontend_compile_result_t. */
typedef struct objc3c_frontend_stage_summary {
  /* Stage identity for this summary record. */
  objc3c_frontend_stage_id_t stage;
  /* Non-zero when this stage was executed. */
  uint8_t attempted;
  /* Non-zero when this stage was intentionally skipped. */
  uint8_t skipped;
  /* Reserved for ABI-compatible field growth; set to 0. */
  uint16_t reserved;
  /* Total diagnostics emitted by this stage. */
  uint32_t diagnostics_total;
  /* Severity breakdown. */
  uint32_t diagnostics_notes;
  uint32_t diagnostics_warnings;
  uint32_t diagnostics_errors;
  uint32_t diagnostics_fatals;
} objc3c_frontend_stage_summary_t;

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_DIAGNOSTIC_H_
