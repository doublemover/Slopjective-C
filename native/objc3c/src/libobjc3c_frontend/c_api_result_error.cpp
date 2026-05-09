#include "libobjc3c_frontend/c_api.h"

extern "C" OBJC3C_FRONTEND_API const objc3c_frontend_c_string_t *
objc3c_frontend_c_result_error_message(
    const objc3c_frontend_c_compile_result_t *result) {
  return result == nullptr ? nullptr : result->error_message;
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_result_error_message_view(
    const objc3c_frontend_c_compile_result_t *result) {
  return objc3c_frontend_c_string_view(
      objc3c_frontend_c_result_error_message(result));
}
