#include "libobjc3c_frontend/objc3c_frontend_result_payload.h"

#include "libobjc3c_frontend/objc3c_frontend_result_ownership.h"

namespace objc3c::frontend {

namespace {

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

void ClearCompileResultPayloadPointers(
    objc3c_frontend_compile_result_t *result) {
  result->error_message = nullptr;
  result->diagnostics_path = nullptr;
  result->manifest_path = nullptr;
  result->runtime_metadata_path = nullptr;
  result->ir_path = nullptr;
  result->object_path = nullptr;
}

}  // namespace

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
    ReleaseCompileResultOwnedStrings(result);
    ClearCompileResultPayloadPointers(result);
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
