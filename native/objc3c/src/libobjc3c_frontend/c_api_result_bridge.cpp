#include "libobjc3c_frontend/c_api.h"

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_c_result_destroy(
    objc3c_frontend_c_compile_result_t *result) {
  objc3c_frontend_result_destroy(result);
}

extern "C" OBJC3C_FRONTEND_API const objc3c_frontend_c_string_t *
objc3c_frontend_c_result_artifact_path(
    const objc3c_frontend_c_compile_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return objc3c_frontend_result_artifact_path(result, artifact_kind);
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_result_artifact_path_view(
    const objc3c_frontend_c_compile_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return objc3c_frontend_c_string_view(
      objc3c_frontend_c_result_artifact_path(result, artifact_kind));
}

extern "C" OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_result_has_artifact(
    const objc3c_frontend_c_compile_result_t *result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  const objc3c_frontend_c_string_view_t view =
      objc3c_frontend_c_result_artifact_path_view(result, artifact_kind);
  return view.data != nullptr && view.size != 0u ? 1u : 0u;
}

extern "C" OBJC3C_FRONTEND_API const objc3c_frontend_c_string_t *
objc3c_frontend_c_result_error_message(
    const objc3c_frontend_c_compile_result_t *result) {
  return objc3c_frontend_result_error_message(result);
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_result_error_message_view(
    const objc3c_frontend_c_compile_result_t *result) {
  return objc3c_frontend_c_string_view(
      objc3c_frontend_c_result_error_message(result));
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_string_view_t
objc3c_frontend_c_string_view(const objc3c_frontend_c_string_t *string) {
  return objc3c_frontend_string_view(string);
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_c_string_release(
    objc3c_frontend_c_string_t *string) {
  objc3c_frontend_string_release(string);
}
