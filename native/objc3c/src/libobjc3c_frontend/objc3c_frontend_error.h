#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_ERROR_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_ERROR_H_

#include <stddef.h>

#include "objc3c_frontend_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Copies the last context error into buffer (always NUL-terminated when buffer_size > 0).
 * Returns required bytes including the NUL terminator; callers can probe required size by
 * passing buffer = NULL or buffer_size = 0.
 * When context is NULL (or no error has been set), returns 1 and writes an empty string.
 */
OBJC3C_FRONTEND_API size_t objc3c_frontend_copy_last_error(
    const objc3c_frontend_context_t *context,
    char *buffer,
    size_t buffer_size);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_ERROR_H_
