#include "libobjc3c_frontend/objc3c_frontend_result_ownership.h"

namespace objc3c::frontend {

void ResetCompileResultForWrite(objc3c_frontend_compile_result_t *result) {
  if (result == nullptr) {
    return;
  }
  *result = {};
}

void ReleaseCompileResultOwnedStrings(objc3c_frontend_compile_result_t *result) {
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

}  // namespace objc3c::frontend
