#include "libobjc3c_frontend/objc3c_frontend_result.h"

#include "libobjc3c_frontend/objc3c_frontend_result_ownership.h"

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_result_destroy(
    objc3c_frontend_compile_result_t *result) {
  if (result == nullptr) {
    return;
  }
  objc3c::frontend::ReleaseCompileResultOwnedStrings(result);
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
