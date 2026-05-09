#ifndef OBJC3C_LIBOBJC3C_FRONTEND_C_API_STRING_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_C_API_STRING_H_

#include "c_api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Standalone string owner for C-only embedders. Views are borrowed from the
 * supplied string. Releasing NULL is a no-op. Result-owned strings are released
 * only by objc3c_frontend_c_result_destroy().
 */
OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_string_view(const objc3c_frontend_c_string_t *string);
OBJC3C_FRONTEND_API void objc3c_frontend_c_string_release(
    objc3c_frontend_c_string_t *string);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_C_API_STRING_H_
