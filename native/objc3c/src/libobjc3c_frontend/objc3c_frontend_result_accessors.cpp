#include "libobjc3c_frontend/objc3c_frontend_result_artifacts.h"

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

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_string_view_t
objc3c_frontend_result_artifact_path_view(
    const objc3c_frontend_compile_result_t *result,
    objc3c_frontend_artifact_kind_t artifact_kind) {
  return objc3c_frontend_string_view(
      objc3c_frontend_result_artifact_path(result, artifact_kind));
}

extern "C" OBJC3C_FRONTEND_API uint8_t objc3c_frontend_result_has_artifact(
    const objc3c_frontend_compile_result_t *result,
    objc3c_frontend_artifact_kind_t artifact_kind) {
  const objc3c_frontend_string_view_t view =
      objc3c_frontend_result_artifact_path_view(result, artifact_kind);
  return view.data != nullptr && view.size != 0u ? 1u : 0u;
}
