#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_STRING_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_STRING_H_

#include <stddef.h>

typedef struct objc3c_frontend_string_view {
  const char *data;
  size_t size;
} objc3c_frontend_string_view_t;

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_STRING_H_
