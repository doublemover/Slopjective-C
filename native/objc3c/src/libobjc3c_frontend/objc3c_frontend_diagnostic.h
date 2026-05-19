#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_DIAGNOSTIC_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_DIAGNOSTIC_H_

#include <stdint.h>

#include "objc3c_frontend_version.h"

/*
 * Deterministic stage identifiers for per-stage summaries in compile results.
 * This header owns caller-visible diagnostic summary metadata; detailed
 * diagnostics are published through the diagnostics artifact path.
 */
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

/*
 * Per-stage execution summary written by value to
 * objc3c_frontend_compile_result_t. Detailed diagnostics payloads are not
 * borrowed from transient pipeline storage; callers read them through the
 * result-owned OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS path when that artifact is
 * produced.
 */
typedef struct objc3c_frontend_stage_summary {
  /* Stage identity for this summary record. */
  objc3c_frontend_stage_id_t stage;
  /* Non-zero when this stage was executed. */
  uint8_t attempted;
  /* Non-zero when this stage was intentionally skipped. */
  uint8_t skipped;
  /* Total diagnostics emitted by this stage. */
  uint32_t diagnostics_total;
  /* Severity breakdown. */
  uint32_t diagnostics_notes;
  uint32_t diagnostics_warnings;
  uint32_t diagnostics_errors;
  uint32_t diagnostics_fatals;
} objc3c_frontend_stage_summary_t;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Public validation helpers for by-value stage summaries. They do not inspect
 * transient diagnostic storage; they validate only the caller-visible record.
 */
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_stage_summary_is_well_formed(
    const objc3c_frontend_stage_summary_t *summary,
    objc3c_frontend_stage_id_t expected_stage);
/* Returns non-zero when the summary records one or more diagnostics. */
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_stage_summary_has_diagnostics(
    const objc3c_frontend_stage_summary_t *summary);
/* Returns non-zero when the summary records error or fatal diagnostics. */
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_stage_summary_has_errors(
    const objc3c_frontend_stage_summary_t *summary);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_DIAGNOSTIC_H_
