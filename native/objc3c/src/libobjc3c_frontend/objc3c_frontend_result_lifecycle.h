#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_LIFECYCLE_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_LIFECYCLE_H_

#include "objc3c_frontend_result_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Compile-result lifetime owner. Destroys only result-owned payload strings and
 * then clears the supplied compile result. Passing NULL is a no-op.
 */
OBJC3C_FRONTEND_API void objc3c_frontend_result_destroy(
    objc3c_frontend_compile_result_t *result);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_RESULT_LIFECYCLE_H_
