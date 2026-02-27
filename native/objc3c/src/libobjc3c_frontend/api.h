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

typedef struct objc3c_frontend_context objc3c_frontend_context_t;

typedef enum objc3c_frontend_stage_id {
  OBJC3C_FRONTEND_STAGE_LEX = 0,
  OBJC3C_FRONTEND_STAGE_PARSE = 1,
  OBJC3C_FRONTEND_STAGE_SEMA = 2,
  OBJC3C_FRONTEND_STAGE_LOWER = 3,
  OBJC3C_FRONTEND_STAGE_EMIT = 4
} objc3c_frontend_stage_id_t;

typedef enum objc3c_frontend_status {
  OBJC3C_FRONTEND_STATUS_OK = 0,
  OBJC3C_FRONTEND_STATUS_DIAGNOSTICS = 1,
  OBJC3C_FRONTEND_STATUS_USAGE_ERROR = 2,
  OBJC3C_FRONTEND_STATUS_EMIT_ERROR = 3,
  OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR = 4
} objc3c_frontend_status_t;

typedef enum objc3c_frontend_diagnostic_severity {
  OBJC3C_FRONTEND_DIAG_NOTE = 0,
  OBJC3C_FRONTEND_DIAG_WARNING = 1,
  OBJC3C_FRONTEND_DIAG_ERROR = 2,
  OBJC3C_FRONTEND_DIAG_FATAL = 3
} objc3c_frontend_diagnostic_severity_t;

typedef struct objc3c_frontend_stage_summary {
  objc3c_frontend_stage_id_t stage;
  uint8_t attempted;
  uint8_t skipped;
  uint16_t reserved;
  uint32_t diagnostics_total;
  uint32_t diagnostics_notes;
  uint32_t diagnostics_warnings;
  uint32_t diagnostics_errors;
  uint32_t diagnostics_fatals;
} objc3c_frontend_stage_summary_t;

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

/* Compatibility contract: callers pass requested ABI and get boolean compatibility result. */
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_is_abi_compatible(uint32_t requested_abi_version);
OBJC3C_FRONTEND_API uint32_t objc3c_frontend_abi_version(void);
OBJC3C_FRONTEND_API objc3c_frontend_version_t objc3c_frontend_version(void);
OBJC3C_FRONTEND_API const char *objc3c_frontend_version_string(void);

OBJC3C_FRONTEND_API objc3c_frontend_context_t *objc3c_frontend_context_create(void);
OBJC3C_FRONTEND_API void objc3c_frontend_context_destroy(objc3c_frontend_context_t *context);

/* CLI parity entrypoint: compile from file path using current native frontend behavior. */
OBJC3C_FRONTEND_API objc3c_frontend_status_t objc3c_frontend_compile_file(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result);

/* Embedding entrypoint: compile from in-memory source text with the same stage contract. */
OBJC3C_FRONTEND_API objc3c_frontend_status_t objc3c_frontend_compile_source(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result);

OBJC3C_FRONTEND_API size_t objc3c_frontend_copy_last_error(
    const objc3c_frontend_context_t *context,
    char *buffer,
    size_t buffer_size);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_API_H_
