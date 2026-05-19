#ifndef OBJC3C_LIBOBJC3C_FRONTEND_C_API_RESULT_LIFECYCLE_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_C_API_RESULT_LIFECYCLE_H_

#include "c_api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * C-only compile-result storage lifetime owner. Destroys only result-owned
 * payload strings and then clears the supplied storage adapter. Passing NULL is
 * a no-op. Opaque result handles are destroyed with
 * objc3c_frontend_c_owned_result_destroy().
 */
OBJC3C_FRONTEND_API void objc3c_frontend_c_result_destroy(
    objc3c_frontend_c_compile_result_t *result);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_C_API_RESULT_LIFECYCLE_H_
