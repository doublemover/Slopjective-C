#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_OPTIONS_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_OPTIONS_H_

#include <stddef.h>
#include <stdint.h>

#define OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3 3u

/* Deterministic IR->object backend selector for emit_object paths. */
typedef enum objc3c_frontend_ir_object_backend {
  OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG = 0,
  OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT = 1
} objc3c_frontend_ir_object_backend_t;

typedef const char *objc3c_frontend_borrowed_c_string_t;
typedef objc3c_frontend_borrowed_c_string_t objc3c_frontend_borrowed_path_t;
typedef objc3c_frontend_borrowed_c_string_t objc3c_frontend_borrowed_text_t;

/*
 * Compile options consumed by objc3c_frontend_compile_file/source.
 * - Borrowed option values are caller-owned for the duration of the call.
 * - borrowed_path_t fields are interpreted as filesystem paths.
 * - borrowed_text_t fields are interpreted as literal source/symbol/prefix text.
 * - compile_file requires non-NULL, non-empty input_path.
 * - compile_source requires non-NULL, non-empty source_text.
 * - emit_manifest, emit_ir, or emit_object requires non-NULL, non-empty
 *   out_dir and emit_prefix and fails before pipeline execution when absent.
 * - emit_object requires clang_path for clang backend and llc_path for
 *   llvm-direct backend and fails before pipeline execution when absent.
 * - language_version must be OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3;
 *   zero is not a default.
 * - allow_live_error_runtime_surface admits the runtime-owned try/throw/throws
 *   proof surface for manifest-only probes; native IR/object emission enables
 *   the same runnable surface automatically.
 * - imported_runtime_surface_paths points at caller-owned borrowed paths and
 *   imported_runtime_surface_path_count names the exact number of entries.
 * - Set unused pointers to NULL.
 */
typedef struct objc3c_frontend_compile_options {
  objc3c_frontend_borrowed_path_t input_path;
  objc3c_frontend_borrowed_text_t source_text;
  objc3c_frontend_borrowed_path_t out_dir;
  objc3c_frontend_borrowed_path_t metaprogramming_cache_root;
  const objc3c_frontend_borrowed_path_t *imported_runtime_surface_paths;
  size_t imported_runtime_surface_path_count;
  objc3c_frontend_borrowed_text_t emit_prefix;
  objc3c_frontend_borrowed_path_t clang_path;
  objc3c_frontend_borrowed_path_t llc_path;
  objc3c_frontend_borrowed_text_t runtime_dispatch_symbol;
  uint32_t max_message_send_args;
  uint8_t emit_manifest;
  uint8_t emit_ir;
  uint8_t emit_object;
  uint8_t ir_object_backend;
  uint8_t language_version;
  uint8_t allow_live_error_runtime_surface;
  uint64_t translation_unit_registration_order_ordinal;
} objc3c_frontend_compile_options_t;

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_OPTIONS_H_
