#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_STRING_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_STRING_H_

#include <stddef.h>

#include "objc3c_frontend_version.h"

/*
 * Owned immutable string returned by libobjc3c_frontend.
 * Release only with objc3c_frontend_string_release().
 */
typedef struct objc3c_frontend_string {
  const char *data;
  size_t size;
} objc3c_frontend_string_t;

/* Borrowed view valid until the owning objc3c_frontend_string_t is released. */
typedef struct objc3c_frontend_string_view {
  const char *data;
  size_t size;
} objc3c_frontend_string_view_t;

#ifdef __cplusplus
extern "C" {
#endif

OBJC3C_FRONTEND_API objc3c_frontend_string_view_t objc3c_frontend_string_view(
    const objc3c_frontend_string_t *string);
OBJC3C_FRONTEND_API void objc3c_frontend_string_release(
    objc3c_frontend_string_t *string);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_STRING_H_
