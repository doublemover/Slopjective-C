#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_STRING_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_STRING_H_

#include <stddef.h>

#include "objc3c_frontend_version.h"

typedef struct objc3c_frontend_string {
  char *data;
  size_t size;
} objc3c_frontend_string_t;

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
