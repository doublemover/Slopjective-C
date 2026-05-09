#include "libobjc3c_frontend/c_api.h"

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_status_t
objc3c_frontend_c_compile_file(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_compile_result_t *result) {
  return objc3c_frontend_compile_file(context, options, result);
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_status_t
objc3c_frontend_c_compile_source(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_compile_result_t *result) {
  return objc3c_frontend_compile_source(context, options, result);
}

extern "C" OBJC3C_FRONTEND_API size_t objc3c_frontend_c_copy_last_error(
    const objc3c_frontend_c_context_t *context,
    char *buffer,
    size_t buffer_size) {
  return objc3c_frontend_copy_last_error(context, buffer, buffer_size);
}
