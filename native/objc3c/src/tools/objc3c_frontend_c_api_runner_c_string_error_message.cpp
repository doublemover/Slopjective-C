#include "tools/objc3c_frontend_c_api_runner_c_string.h"

FrontendCApiRunnerStringSnapshot FrontendCApiResultErrorMessageSnapshot(
    const objc3c_frontend_c_compile_result_t &result) {
  return SnapshotOptionalFrontendCApiString(
      objc3c_frontend_c_result_error_message(&result));
}

std::string FrontendCApiResultErrorMessage(
    const objc3c_frontend_c_compile_result_t &result) {
  return FrontendCApiResultErrorMessageSnapshot(result).text;
}
