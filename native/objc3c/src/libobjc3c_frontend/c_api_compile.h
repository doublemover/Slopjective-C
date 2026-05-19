#ifndef OBJC3C_LIBOBJC3C_FRONTEND_C_API_COMPILE_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_C_API_COMPILE_H_

#include <stddef.h>

#include "c_api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Compile/error owner for C-only embedders. Entry points require non-NULL
 * context, options, and result pointers. The C API layer rejects NULL inputs
 * before crossing into the native compile pipeline, records deterministic
 * result-owned error messages when result output is available, and never uses a
 * retired route compile path.
 */
OBJC3C_FRONTEND_API objc3c_frontend_c_status_t objc3c_frontend_c_compile_file(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_compile_result_t *result);

OBJC3C_FRONTEND_API objc3c_frontend_c_status_t
objc3c_frontend_c_compile_source(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_compile_result_t *result);

OBJC3C_FRONTEND_API size_t objc3c_frontend_c_copy_last_error(
    const objc3c_frontend_c_context_t *context,
    char *buffer,
    size_t buffer_size);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_C_API_COMPILE_H_
