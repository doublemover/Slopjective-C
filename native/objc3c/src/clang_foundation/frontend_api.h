#ifndef OBJC3C_CLANG_FOUNDATION_FRONTEND_API_H
#define OBJC3C_CLANG_FOUNDATION_FRONTEND_API_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OBJC3C_FRONTEND_V2_ABI 2u
typedef struct objc3c_frontend_v2_result objc3c_frontend_v2_result;

/* Size/versioned addition: no existing caller-allocated struct changes. All
 * input strings are UTF-8 and borrowed until compile returns. Source bytes may
 * be empty; NULL source selects file-backed compilation. Language is 0=C,
 * 1=C++, 2=ObjC, 3=ObjC++; C18/C++20 are defaults and explicit -std options
 * are preserved. emit is 0=syntax, 1=object, 2=LLVM text. Profile
 * is 0=compatibility, 1=strict, 2=strict concurrency, 3=strict system.
 * The result owns diagnostics/strings and must be destroyed exactly once. */
typedef struct objc3c_frontend_v2_options {
  uint32_t struct_size;
  uint32_t abi_version;
  const char *input_path;
  const char *source;
  size_t source_size;
  const char *working_directory;
  const char *output_path;
  const char *target_triple;
  const char *objc_runtime;
  const char *const *arguments;
  size_t argument_count;
  uint32_t language;
  uint32_t emit;
  uint32_t profile;
  uint8_t objc3;
  uint8_t arc;
  uint8_t strict_concurrency;
  uint8_t emit_semantic_interface;
  const char *module_name;
  const char *package_identity;
} objc3c_frontend_v2_options;

typedef struct objc3c_frontend_v2_location {
  const char *file;
  uint32_t line;
  uint32_t column;
} objc3c_frontend_v2_location;

/* 0 success, 1 Clang diagnostics/emission failure, 2 invalid ABI/options,
 * 3 host allocation/internal failure. out_result is cleared on entry. */
int objc3c_frontend_v2_compile(const objc3c_frontend_v2_options *options,
                             objc3c_frontend_v2_result **out_result);
void objc3c_frontend_v2_result_destroy(objc3c_frontend_v2_result *result);
int objc3c_frontend_v2_result_success(const objc3c_frontend_v2_result *result);
const char *objc3c_frontend_v2_result_failure(const objc3c_frontend_v2_result *result);
const char *objc3c_frontend_v2_result_identity(const objc3c_frontend_v2_result *result);
size_t objc3c_frontend_v2_diagnostic_count(const objc3c_frontend_v2_result *result);
const char *objc3c_frontend_v2_diagnostic_message(
    const objc3c_frontend_v2_result *result, size_t index);
unsigned objc3c_frontend_v2_diagnostic_id(const objc3c_frontend_v2_result *result,
                                       size_t index);
unsigned objc3c_frontend_v2_diagnostic_severity(
    const objc3c_frontend_v2_result *result, size_t index);
objc3c_frontend_v2_location objc3c_frontend_v2_diagnostic_spelling(
    const objc3c_frontend_v2_result *result, size_t index);
objc3c_frontend_v2_location objc3c_frontend_v2_diagnostic_expansion(
    const objc3c_frontend_v2_result *result, size_t index);
/* Owned UTF-8 paired artifact bytes are borrowed until result destruction;
 * NULL is returned unless requested emission and compilation both succeeded. */
const char *objc3c_frontend_v2_interface_source(const objc3c_frontend_v2_result *result);
size_t objc3c_frontend_v2_interface_source_size(const objc3c_frontend_v2_result *result);
const char *objc3c_frontend_v2_interface_sidecar(const objc3c_frontend_v2_result *result);
size_t objc3c_frontend_v2_interface_sidecar_size(const objc3c_frontend_v2_result *result);

#ifdef __cplusplus
}
#endif
#endif
