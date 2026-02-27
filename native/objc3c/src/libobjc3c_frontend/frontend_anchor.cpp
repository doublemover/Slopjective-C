#include "libobjc3c_frontend/api.h"

#include <algorithm>
#include <cstring>
#include <new>
#include <string>

struct objc3c_frontend_context {
  std::string last_error;
};

static void objc3c_frontend_set_error(objc3c_frontend_context_t *context, const char *message) {
  if (context == nullptr) {
    return;
  }
  context->last_error = message == nullptr ? "" : message;
}

extern "C" OBJC3C_FRONTEND_API uint8_t objc3c_frontend_is_abi_compatible(uint32_t requested_abi_version) {
  return requested_abi_version >= OBJC3C_FRONTEND_MIN_COMPATIBILITY_ABI_VERSION &&
                 requested_abi_version <= OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION
             ? 1u
             : 0u;
}

extern "C" OBJC3C_FRONTEND_API uint32_t objc3c_frontend_abi_version(void) {
  return OBJC3C_FRONTEND_ABI_VERSION;
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_version_t objc3c_frontend_version(void) {
  return (objc3c_frontend_version_t)OBJC3C_FRONTEND_VERSION_INIT;
}

extern "C" OBJC3C_FRONTEND_API const char *objc3c_frontend_version_string(void) {
  return OBJC3C_FRONTEND_VERSION_STRING;
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_context_t *objc3c_frontend_context_create(void) {
  objc3c_frontend_context_t *context = new (std::nothrow) objc3c_frontend_context_t();
  if (context != nullptr) {
    context->last_error.clear();
  }
  return context;
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_context_destroy(objc3c_frontend_context_t *context) {
  delete context;
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_status_t objc3c_frontend_compile_file(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result) {
  if (context == nullptr || options == nullptr || result == nullptr) {
    return OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
  }
  *result = {};
  result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
  objc3c_frontend_set_error(
      context,
      "libobjc3c_frontend compile entrypoints are scaffolded only; use objc3c-native CLI until pipeline wiring lands.");
  return result->status;
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_status_t objc3c_frontend_compile_source(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result) {
  if (context == nullptr || options == nullptr || result == nullptr) {
    return OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
  }
  *result = {};
  result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
  objc3c_frontend_set_error(
      context,
      "libobjc3c_frontend compile entrypoints are scaffolded only; use objc3c-native CLI until pipeline wiring lands.");
  return result->status;
}

extern "C" OBJC3C_FRONTEND_API size_t objc3c_frontend_copy_last_error(
    const objc3c_frontend_context_t *context,
    char *buffer,
    size_t buffer_size) {
  const std::string message = context == nullptr ? "" : context->last_error;
  const size_t required = message.size() + 1;

  if (buffer == nullptr || buffer_size == 0) {
    return required;
  }

  const size_t bytes_to_copy = std::min(required - 1, buffer_size - 1);
  if (bytes_to_copy > 0) {
    std::memcpy(buffer, message.data(), bytes_to_copy);
  }
  buffer[bytes_to_copy] = '\0';
  return required;
}
