#include "libobjc3c_frontend/objc3c_frontend_result_ownership.h"

#include <cstdlib>
#include <cstring>

namespace objc3c::frontend {

void ResetCompileResultForWrite(objc3c_frontend_compile_result_t *result) {
  if (result == nullptr) {
    return;
  }
  *result = {};
}

objc3c_frontend_string_t *CloneOwnedFrontendString(const std::string &text) {
  if (text.empty()) {
    return nullptr;
  }

  auto *string = static_cast<objc3c_frontend_string_t *>(
      std::malloc(sizeof(objc3c_frontend_string_t)));
  if (string == nullptr) {
    return nullptr;
  }

  auto *data = static_cast<char *>(std::malloc(text.size() + 1u));
  if (data == nullptr) {
    std::free(string);
    return nullptr;
  }

  std::memcpy(data, text.data(), text.size());
  data[text.size()] = '\0';
  string->data = data;
  string->size = text.size();
  return string;
}

void ReleaseOwnedFrontendString(objc3c_frontend_string_t *string) {
  if (string == nullptr) {
    return;
  }
  std::free(const_cast<char *>(string->data));
  std::free(string);
}

bool AssignOwnedString(const std::string &source,
                       objc3c_frontend_string_t *&target,
                       std::string &error) {
  target = nullptr;
  if (source.empty()) {
    return true;
  }

  target = CloneOwnedFrontendString(source);
  if (target == nullptr) {
    error = "failed to allocate frontend result-owned string storage.";
    return false;
  }
  return true;
}

void ReleaseResultOwnedStrings(objc3c_frontend_compile_result_t *result) {
  if (result == nullptr) {
    return;
  }
  ReleaseOwnedFrontendString(result->error_message);
  ReleaseOwnedFrontendString(result->diagnostics_path);
  ReleaseOwnedFrontendString(result->manifest_path);
  ReleaseOwnedFrontendString(result->runtime_metadata_path);
  ReleaseOwnedFrontendString(result->ir_path);
  ReleaseOwnedFrontendString(result->object_path);
}

bool PopulateCompileResultOwnedPayload(
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendResultOwnedPayload &payload,
    std::string &error) {
  if (result == nullptr) {
    error = "compile result pointer is null.";
    return false;
  }

  if (!AssignOwnedString(payload.error_message, result->error_message, error) ||
      !AssignOwnedString(payload.diagnostics_path,
                         result->diagnostics_path,
                         error) ||
      !AssignOwnedString(payload.manifest_path, result->manifest_path, error) ||
      !AssignOwnedString(payload.runtime_metadata_path,
                         result->runtime_metadata_path,
                         error) ||
      !AssignOwnedString(payload.ir_path, result->ir_path, error) ||
      !AssignOwnedString(payload.object_path, result->object_path, error)) {
    ReleaseResultOwnedStrings(result);
    result->error_message = nullptr;
    result->diagnostics_path = nullptr;
    result->manifest_path = nullptr;
    result->runtime_metadata_path = nullptr;
    result->ir_path = nullptr;
    result->object_path = nullptr;
    return false;
  }

  return true;
}

bool SetCompileResultOwnedErrorMessage(
    objc3c_frontend_compile_result_t *result,
    const std::string &message,
    std::string &error) {
  if (result == nullptr) {
    error = "compile result pointer is null.";
    return false;
  }
  return AssignOwnedString(message, result->error_message, error);
}

}  // namespace objc3c::frontend

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_string_view_t
objc3c_frontend_string_view(const objc3c_frontend_string_t *string) {
  if (string == nullptr || string->data == nullptr) {
    return {nullptr, 0u};
  }
  return {string->data, string->size};
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_string_release(
    objc3c_frontend_string_t *string) {
  objc3c::frontend::ReleaseOwnedFrontendString(string);
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_result_destroy(
    objc3c_frontend_compile_result_t *result) {
  if (result == nullptr) {
    return;
  }
  objc3c::frontend::ReleaseResultOwnedStrings(result);
  *result = {};
}

extern "C" OBJC3C_FRONTEND_API const objc3c_frontend_string_t *
objc3c_frontend_result_artifact_path(
    const objc3c_frontend_compile_result_t *result,
    objc3c_frontend_artifact_kind_t artifact_kind) {
  if (result == nullptr) {
    return nullptr;
  }

  switch (artifact_kind) {
    case OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS:
      return result->diagnostics_path;
    case OBJC3C_FRONTEND_ARTIFACT_MANIFEST:
      return result->manifest_path;
    case OBJC3C_FRONTEND_ARTIFACT_IR:
      return result->ir_path;
    case OBJC3C_FRONTEND_ARTIFACT_OBJECT:
      return result->object_path;
    case OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA:
      return result->runtime_metadata_path;
  }

  return nullptr;
}

extern "C" OBJC3C_FRONTEND_API const objc3c_frontend_string_t *
objc3c_frontend_result_error_message(
    const objc3c_frontend_compile_result_t *result) {
  return result == nullptr ? nullptr : result->error_message;
}
