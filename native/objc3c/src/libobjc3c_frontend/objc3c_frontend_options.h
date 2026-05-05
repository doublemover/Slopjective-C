#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_OPTIONS_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_OPTIONS_H_

#include <stdint.h>

#define OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3 3u
#define OBJC3C_FRONTEND_LANGUAGE_VERSION_DEFAULT OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3

/* Deterministic IR->object backend selector for emit_object paths. */
typedef enum objc3c_frontend_ir_object_backend {
  OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG = 0,
  OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT = 1
} objc3c_frontend_ir_object_backend_t;

/*
 * Compile options consumed by objc3c_frontend_compile_file/source.
 * - input_path is used by file-backed workflows.
 * - source_text is used by in-memory workflows.
 * - language_version uses Objective-C version 3 by default when set to 0.
 * - Set unused pointers to NULL and reserved fields to 0.
 */
typedef struct objc3c_frontend_compile_options {
  const char *input_path;
  const char *source_text;
  const char *out_dir;
  const char *emit_prefix;
  const char *clang_path;
  const char *llc_path;
  const char *runtime_dispatch_symbol;
  uint32_t max_message_send_args;
  uint8_t emit_manifest;
  uint8_t emit_ir;
  uint8_t emit_object;
  uint8_t ir_object_backend;
  uint8_t language_version;
  uint8_t reserved0;
  uint8_t reserved1;
  uint8_t reserved2;
  uint64_t translation_unit_registration_order_ordinal;
} objc3c_frontend_compile_options_t;

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_OPTIONS_H_
