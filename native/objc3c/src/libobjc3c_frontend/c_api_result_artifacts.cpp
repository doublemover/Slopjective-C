#include "libobjc3c_frontend/c_api.h"

extern "C" OBJC3C_FRONTEND_API const objc3c_frontend_c_string_t *
objc3c_frontend_c_result_artifact_path(
    const objc3c_frontend_c_compile_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
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

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_result_artifact_path_view(
    const objc3c_frontend_c_compile_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return objc3c_frontend_result_artifact_path_view(result, artifact_kind);
}

extern "C" OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_result_has_artifact(
    const objc3c_frontend_c_compile_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return objc3c_frontend_result_has_artifact(result, artifact_kind);
}
