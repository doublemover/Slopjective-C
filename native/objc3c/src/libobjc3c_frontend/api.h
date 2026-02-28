#ifndef OBJC3C_LIBOBJC3C_FRONTEND_API_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_API_H_

#include <stddef.h>
#include <stdint.h>

#include "version.h"

#ifndef OBJC3C_FRONTEND_API
#if defined(_WIN32)
#if defined(OBJC3C_FRONTEND_BUILD_DLL)
#define OBJC3C_FRONTEND_API __declspec(dllexport)
#elif defined(OBJC3C_FRONTEND_USE_DLL)
#define OBJC3C_FRONTEND_API __declspec(dllimport)
#else
#define OBJC3C_FRONTEND_API
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define OBJC3C_FRONTEND_API __attribute__((visibility("default")))
#else
#define OBJC3C_FRONTEND_API
#endif
#endif

#ifndef OBJC3C_FRONTEND_DEPRECATED
#if defined(__GNUC__) || defined(__clang__)
#define OBJC3C_FRONTEND_DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#define OBJC3C_FRONTEND_DEPRECATED(msg) __declspec(deprecated(msg))
#else
#define OBJC3C_FRONTEND_DEPRECATED(msg)
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Public embedding ABI contract:
 * - This header defines the exported symbol and struct-layout surface for libobjc3c_frontend.
 * - Callers should gate startup with objc3c_frontend_is_abi_compatible().
 * - Reserved struct fields are for forward ABI growth and should be zero-initialized by callers.
 * - ABI evolution policy for exposed structs/enums is additive; existing fields and values remain stable.
 */
typedef struct objc3c_frontend_context objc3c_frontend_context_t;

/* Deterministic stage identifiers for per-stage summaries in compile results. */
typedef enum objc3c_frontend_stage_id {
  OBJC3C_FRONTEND_STAGE_LEX = 0,
  OBJC3C_FRONTEND_STAGE_PARSE = 1,
  OBJC3C_FRONTEND_STAGE_SEMA = 2,
  OBJC3C_FRONTEND_STAGE_LOWER = 3,
  OBJC3C_FRONTEND_STAGE_EMIT = 4
} objc3c_frontend_stage_id_t;

/* Top-level compile status values returned by compile entrypoints. */
typedef enum objc3c_frontend_status {
  OBJC3C_FRONTEND_STATUS_OK = 0,
  OBJC3C_FRONTEND_STATUS_DIAGNOSTICS = 1,
  OBJC3C_FRONTEND_STATUS_USAGE_ERROR = 2,
  OBJC3C_FRONTEND_STATUS_EMIT_ERROR = 3,
  OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR = 4
} objc3c_frontend_status_t;

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

/*
 * Compile options consumed by objc3c_frontend_compile_file/source.
 * - input_path is used by file-backed workflows.
 * - source_text is used by in-memory workflows.
 * - Set unused pointers to NULL and reserved fields to 0.
 */
typedef struct objc3c_frontend_compile_options {
  const char *input_path;
  const char *source_text;
  const char *out_dir;
  const char *emit_prefix;
  const char *clang_path;
  const char *runtime_dispatch_symbol;
  uint32_t max_message_send_args;
  uint8_t emit_manifest;
  uint8_t emit_ir;
  uint8_t emit_object;
  uint8_t reserved0;
} objc3c_frontend_compile_options_t;

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

/*
 * Returns 1 when requested_abi_version is in the inclusive compatibility window
 * [OBJC3C_FRONTEND_MIN_COMPATIBILITY_ABI_VERSION, OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION].
 */
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_is_abi_compatible(uint32_t requested_abi_version);
/* Returns the library ABI version encoded in this build. */
OBJC3C_FRONTEND_API uint32_t objc3c_frontend_abi_version(void);
/* Returns semantic version + ABI tuple for this build. */
OBJC3C_FRONTEND_API objc3c_frontend_version_t objc3c_frontend_version(void);
/* Returns a static process-lifetime SemVer string (for example "0.1.0"). */
OBJC3C_FRONTEND_API const char *objc3c_frontend_version_string(void);

/* Creates an embedding context. Returns NULL on allocation failure. */
OBJC3C_FRONTEND_API objc3c_frontend_context_t *objc3c_frontend_context_create(void);
/* Destroys a context created by objc3c_frontend_context_create(). */
OBJC3C_FRONTEND_API void objc3c_frontend_context_destroy(objc3c_frontend_context_t *context);

/*
 * Compile entrypoint for file-backed embedding.
 * Pipeline-backed behavior:
 * - Runs lexer/parser/sema/lower/emit through the extracted frontend pipeline.
 * - Writes selected artifacts to out_dir (when provided) based on emit flags.
 * - Returns OBJC3C_FRONTEND_STATUS_DIAGNOSTICS on source diagnostics,
 *   OBJC3C_FRONTEND_STATUS_EMIT_ERROR on object emission failures,
 *   and OBJC3C_FRONTEND_STATUS_USAGE_ERROR for invalid arguments.
 */
OBJC3C_FRONTEND_API objc3c_frontend_status_t objc3c_frontend_compile_file(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result);

/*
 * Compile entrypoint for in-memory source embedding.
 * Pipeline-backed behavior mirrors objc3c_frontend_compile_file and accepts
 * compile_options.source_text as the source input.
 */
OBJC3C_FRONTEND_API objc3c_frontend_status_t objc3c_frontend_compile_source(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result);

/*
 * Copies the last context error into buffer (always NUL-terminated when buffer_size > 0).
 * Returns required bytes including the NUL terminator; callers can probe required size by
 * passing buffer = NULL or buffer_size = 0.
 * When context is NULL (or no error has been set), returns 1 and writes an empty string.
 */
OBJC3C_FRONTEND_API size_t objc3c_frontend_copy_last_error(
    const objc3c_frontend_context_t *context,
    char *buffer,
    size_t buffer_size);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_API_H_
