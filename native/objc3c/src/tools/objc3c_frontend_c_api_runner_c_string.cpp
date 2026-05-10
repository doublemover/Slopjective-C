#include "tools/objc3c_frontend_c_api_runner_c_string.h"

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
