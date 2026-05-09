#include "tools/objc3c_frontend_c_api_runner_c_string.h"

#include <cstddef>

FrontendCApiRunnerStringSnapshot SnapshotOptionalFrontendCApiString(
    const objc3c_frontend_c_string_t *value) {
  FrontendCApiRunnerStringSnapshot snapshot;
  if (value == nullptr) {
    return snapshot;
  }
  snapshot.present = true;
  const objc3c_frontend_c_string_view_t view =
      objc3c_frontend_c_string_view(value);
  if (view.data == nullptr || view.size == 0) {
    return snapshot;
  }
  snapshot.text = std::string(view.data, view.size);
  return snapshot;
}

std::string OptionalFrontendCApiString(
    const objc3c_frontend_c_string_t *value) {
  return SnapshotOptionalFrontendCApiString(value).text;
}

FrontendCApiRunnerStringSnapshot FrontendCApiResultArtifactPathSnapshot(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return SnapshotOptionalFrontendCApiString(
      objc3c_frontend_c_result_artifact_path(&result, artifact_kind));
}

std::string FrontendCApiResultArtifactPath(
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_artifact_kind_t artifact_kind) {
  return FrontendCApiResultArtifactPathSnapshot(result, artifact_kind).text;
}

FrontendCApiRunnerStringSnapshot FrontendCApiResultErrorMessageSnapshot(
    const objc3c_frontend_c_compile_result_t &result) {
  return SnapshotOptionalFrontendCApiString(
      objc3c_frontend_c_result_error_message(&result));
}

std::string FrontendCApiResultErrorMessage(
    const objc3c_frontend_c_compile_result_t &result) {
  return FrontendCApiResultErrorMessageSnapshot(result).text;
}

std::string ReadFrontendCApiLastError(
    const objc3c_frontend_c_context_t *context) {
  const std::size_t required =
      objc3c_frontend_c_copy_last_error(context, nullptr, 0);
  if (required == 0) {
    return "";
  }
  std::string message(required, '\0');
  const std::size_t written = objc3c_frontend_c_copy_last_error(
      context, message.data(), message.size());
  if (written == 0) {
    return "";
  }
  if (!message.empty() && message.back() == '\0') {
    message.pop_back();
  }
  return message;
}
