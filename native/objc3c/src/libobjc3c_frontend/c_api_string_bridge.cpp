#include "libobjc3c_frontend/c_api.h"

#include "libobjc3c_frontend/objc3c_frontend_result_ownership.h"

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_string_view(const objc3c_frontend_c_string_t *string) {
  if (string == nullptr || string->data == nullptr) {
    return {nullptr, 0u};
  }
  return {string->data, string->size};
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_c_string_release(
    objc3c_frontend_c_string_t *string) {
  objc3c::frontend::ReleaseOwnedFrontendString(string);
}
