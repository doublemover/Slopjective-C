#ifndef OBJC3C_LIBOBJC3C_FRONTEND_C_API_RESULT_ERROR_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_C_API_RESULT_ERROR_H_

#include "c_api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * C-only result-owned error payload accessors. Returned strings/views are
 * borrowed from result storage and remain valid only until result destruction.
 */
OBJC3C_FRONTEND_API const objc3c_frontend_c_string_t *
objc3c_frontend_c_result_error_message(
    const objc3c_frontend_c_compile_result_t *result);
OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_result_error_message_view(
    const objc3c_frontend_c_compile_result_t *result);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_C_API_RESULT_ERROR_H_
