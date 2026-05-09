#include "libobjc3c_frontend/objc3c_frontend_result_lifecycle.h"

#include "libobjc3c_frontend/objc3c_frontend_result_ownership.h"

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_result_destroy(
    objc3c_frontend_compile_result_t *result) {
  if (result == nullptr) {
    return;
  }
  objc3c::frontend::ReleaseCompileResultOwnedStrings(result);
  *result = {};
}
