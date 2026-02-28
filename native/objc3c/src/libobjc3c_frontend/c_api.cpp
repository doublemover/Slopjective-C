#include "libobjc3c_frontend/c_api.h"

#include <type_traits>

static_assert(OBJC3C_FRONTEND_C_API_ABI_VERSION == 1u, "unexpected c api wrapper abi version");
static_assert(std::is_same_v<objc3c_frontend_c_context_t, objc3c_frontend_context_t>,
              "context alias mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t, objc3c_frontend_compile_options_t>,
              "compile options alias mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_compile_result_t, objc3c_frontend_compile_result_t>,
              "compile result alias mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_version_t, objc3c_frontend_version_t>,
              "version alias mismatch");

extern "C" OBJC3C_FRONTEND_API uint32_t objc3c_frontend_c_api_abi_version(void) {
  return OBJC3C_FRONTEND_C_API_ABI_VERSION;
}

extern "C" OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_is_abi_compatible(uint32_t requested_abi_version) {
  return objc3c_frontend_is_abi_compatible(requested_abi_version);
}

extern "C" OBJC3C_FRONTEND_API uint32_t objc3c_frontend_c_abi_version(void) {
  return objc3c_frontend_abi_version();
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_version_t objc3c_frontend_c_version(void) {
  return objc3c_frontend_version();
}

extern "C" OBJC3C_FRONTEND_API const char *objc3c_frontend_c_version_string(void) {
  return objc3c_frontend_version_string();
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_context_t *objc3c_frontend_c_context_create(void) {
  return objc3c_frontend_context_create();
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_c_context_destroy(objc3c_frontend_c_context_t *context) {
  objc3c_frontend_context_destroy(context);
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_status_t objc3c_frontend_c_compile_file(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_compile_result_t *result) {
  return objc3c_frontend_compile_file(context, options, result);
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_status_t objc3c_frontend_c_compile_source(
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
