#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_STRING_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_STRING_H_

#include <stddef.h>

#include "objc3c_frontend_version.h"

/*
 * Owned immutable string returned by libobjc3c_frontend. Strings returned as
 * members of objc3c_frontend_compile_result_t are result-owned and released
 * only by objc3c_frontend_result_destroy(); standalone owned strings returned
 * by a public function are released with objc3c_frontend_string_release().
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
/* Releases standalone owned strings. Passing NULL is a no-op. */
OBJC3C_FRONTEND_API void objc3c_frontend_string_release(
    objc3c_frontend_string_t *string);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_STRING_H_
