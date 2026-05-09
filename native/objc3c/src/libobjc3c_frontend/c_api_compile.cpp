#include "libobjc3c_frontend/c_api.h"

#include <string>

#include "libobjc3c_frontend/objc3c_frontend_context_state.h"

namespace {

objc3c_frontend_c_status_t ValidateFrontendCApiCompileArguments(
    const char *entrypoint,
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_compile_result_t *result,
    bool &accepted) {
  accepted = false;
  if (result == nullptr) {
    return OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
  }
  if (context == nullptr) {
    return objc3c::frontend::SetFrontendUsageErrorWithoutContext(
        result, std::string(entrypoint) + " requires a frontend context.");
  }
  if (options == nullptr) {
    return objc3c::frontend::SetFrontendUsageError(
        context, result,
        std::string(entrypoint) + " requires compile options.");
  }
  accepted = true;
  return OBJC3C_FRONTEND_STATUS_OK;
}

}  // namespace

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_status_t
objc3c_frontend_c_compile_file(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_compile_result_t *result) {
  bool accepted = false;
  const objc3c_frontend_c_status_t status =
      ValidateFrontendCApiCompileArguments("objc3c_frontend_c_compile_file",
                                           context, options, result, accepted);
  if (!accepted) {
    return status;
  }
  return objc3c_frontend_compile_file(context, options, result);
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_status_t
objc3c_frontend_c_compile_source(
    objc3c_frontend_c_context_t *context,
    const objc3c_frontend_c_compile_options_t *options,
    objc3c_frontend_c_compile_result_t *result) {
  bool accepted = false;
  const objc3c_frontend_c_status_t status =
      ValidateFrontendCApiCompileArguments("objc3c_frontend_c_compile_source",
                                           context, options, result, accepted);
  if (!accepted) {
    return status;
  }
  return objc3c_frontend_compile_source(context, options, result);
}

extern "C" OBJC3C_FRONTEND_API size_t objc3c_frontend_c_copy_last_error(
    const objc3c_frontend_c_context_t *context,
    char *buffer,
    size_t buffer_size) {
  return objc3c::frontend::CopyFrontendContextLastError(context, buffer,
                                                        buffer_size);
}
