#ifndef OBJC3C_LIBOBJC3C_FRONTEND_C_API_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_C_API_H_

#include <stddef.h>
#include <stdint.h>

#include "api.h"

#define OBJC3C_FRONTEND_C_API_ABI_VERSION 1u

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Optional C ABI shim for non-C++ embedding environments.
 * This wrapper preserves the underlying objc3c_frontend ABI data structures
 * and forwards to the primary libobjc3c_frontend entrypoints.
 */
typedef objc3c_frontend_context_t objc3c_frontend_c_context_t;
typedef objc3c_frontend_stage_id_t objc3c_frontend_c_stage_id_t;
typedef objc3c_frontend_status_t objc3c_frontend_c_status_t;
typedef objc3c_frontend_diagnostic_severity_t objc3c_frontend_c_diagnostic_severity_t;
typedef objc3c_frontend_stage_summary_t objc3c_frontend_c_stage_summary_t;
typedef objc3c_frontend_compile_options_t objc3c_frontend_c_compile_options_t;
typedef objc3c_frontend_compile_result_t objc3c_frontend_c_compile_result_t;
typedef objc3c_frontend_version_t objc3c_frontend_c_version_t;

OBJC3C_FRONTEND_API uint32_t objc3c_frontend_c_api_abi_version(void);

OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_is_abi_compatible(
    uint32_t requested_abi_version);
OBJC3C_FRONTEND_API uint32_t objc3c_frontend_c_abi_version(void);
OBJC3C_FRONTEND_API objc3c_frontend_c_version_t objc3c_frontend_c_version(void);
OBJC3C_FRONTEND_API const char *objc3c_frontend_c_version_string(void);

OBJC3C_FRONTEND_API objc3c_frontend_c_context_t *objc3c_frontend_c_context_create(void);
OBJC3C_FRONTEND_API void objc3c_frontend_c_context_destroy(objc3c_frontend_c_context_t *context);

OBJC3C_FRONTEND_API objc3c_frontend_c_status_t objc3c_frontend_c_compile_file(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_compile_result_t *result);

OBJC3C_FRONTEND_API objc3c_frontend_c_status_t objc3c_frontend_c_compile_source(
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

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_C_API_H_
