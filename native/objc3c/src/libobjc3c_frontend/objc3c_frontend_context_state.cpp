#include "libobjc3c_frontend/objc3c_frontend_context_state.h"

#include <algorithm>
#include <cstring>
#include <new>

#include "libobjc3c_frontend/objc3c_frontend_result_payload.h"
#include "libobjc3c_frontend/objc3c_frontend_result_ownership.h"

namespace objc3c::frontend {

std::mutex &FrontendContextMutex(objc3c_frontend_context_t *context) {
  return context->mutex;
}

void SetFrontendContextError(objc3c_frontend_context_t *context,
                             const char *message) {
  if (context == nullptr) {
    return;
  }
  context->last_error = message == nullptr ? "" : message;
}

void ClearFrontendContextResultPaths(objc3c_frontend_context_t *context) {
  if (context == nullptr) {
    return;
  }
  context->diagnostics_path.clear();
  context->manifest_path.clear();
  context->runtime_metadata_binary_path.clear();
  context->ir_path.clear();
  context->object_path.clear();
}

void SetFrontendContextDiagnosticsPath(objc3c_frontend_context_t *context,
                                       const std::string &path) {
  if (context != nullptr) {
    context->diagnostics_path = path;
  }
}

void SetFrontendContextManifestPath(objc3c_frontend_context_t *context,
                                    const std::string &path) {
  if (context != nullptr) {
    context->manifest_path = path;
  }
}

void SetFrontendContextRuntimeMetadataPath(objc3c_frontend_context_t *context,
                                           const std::string &path) {
  if (context != nullptr) {
    context->runtime_metadata_binary_path = path;
  }
}

void SetFrontendContextIrPath(objc3c_frontend_context_t *context,
                              const std::string &path) {
  if (context != nullptr) {
    context->ir_path = path;
  }
}

void SetFrontendContextObjectPath(objc3c_frontend_context_t *context,
                                  const std::string &path) {
  if (context != nullptr) {
    context->object_path = path;
  }
}

bool PopulateCompileResultFromFrontendContext(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    std::string &error) {
  if (context == nullptr || result == nullptr) {
    error = "result path population requires context and result output.";
    return false;
  }

  Objc3FrontendResultOwnedPayload payload;
  payload.error_message = context->last_error;
  payload.diagnostics_path = context->diagnostics_path;
  payload.manifest_path = context->manifest_path;
  payload.runtime_metadata_path = context->runtime_metadata_binary_path;
  payload.ir_path = context->ir_path;
  payload.object_path = context->object_path;
  return PopulateCompileResultOwnedPayload(result, payload, error);
}

objc3c_frontend_status_t ReturnWithFrontendContextResultPayload(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result) {
  std::string result_ownership_error;
  if (!PopulateCompileResultFromFrontendContext(context, result,
                                                result_ownership_error)) {
    result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
    result->process_exit_code = 2;
    result->success = 0;
    SetFrontendContextError(context, result_ownership_error.c_str());
  }
  return result->status;
}

objc3c_frontend_status_t SetFrontendUsageError(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const std::string &message) {
  ResetCompileResultForWrite(result);
  result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
  result->process_exit_code = 2;
  result->success = 0;
  std::string ownership_error;
  if (!SetCompileResultOwnedErrorMessage(result, message, ownership_error)) {
    result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
    result->process_exit_code = 2;
    SetFrontendContextError(context, ownership_error.c_str());
    return result->status;
  }
  SetFrontendContextError(context, message.c_str());
  return result->status;
}

objc3c_frontend_status_t SetFrontendUsageErrorWithoutContext(
    objc3c_frontend_compile_result_t *result,
    const std::string &message) {
  if (result != nullptr) {
    ResetCompileResultForWrite(result);
    result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
    result->process_exit_code = 2;
    result->success = 0;
    std::string ownership_error;
    if (!SetCompileResultOwnedErrorMessage(result, message, ownership_error)) {
      result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
      result->process_exit_code = 2;
    }
    return result->status;
  }
  return OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
}

size_t CopyFrontendContextLastError(const objc3c_frontend_context_t *context,
                                    char *buffer,
                                    size_t buffer_size) {
  std::unique_lock<std::mutex> lock;
  if (context != nullptr) {
    lock = std::unique_lock<std::mutex>(context->mutex);
  }
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

}  // namespace objc3c::frontend

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_context_t *
objc3c_frontend_context_create(void) {
  objc3c_frontend_context_t *context =
      new (std::nothrow) objc3c_frontend_context_t();
  if (context != nullptr) {
    context->last_error.clear();
  }
  return context;
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_context_destroy(
    objc3c_frontend_context_t *context) {
  delete context;
}

extern "C" OBJC3C_FRONTEND_API size_t objc3c_frontend_copy_last_error(
    const objc3c_frontend_context_t *context,
    char *buffer,
    size_t buffer_size) {
  return objc3c::frontend::CopyFrontendContextLastError(context, buffer,
                                                        buffer_size);
}
