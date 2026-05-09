#include "libobjc3c_frontend/c_api.h"

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_string_view(const objc3c_frontend_c_string_t *string) {
  return objc3c_frontend_string_view(string);
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_c_string_release(
    objc3c_frontend_c_string_t *string) {
  objc3c_frontend_string_release(string);
}
