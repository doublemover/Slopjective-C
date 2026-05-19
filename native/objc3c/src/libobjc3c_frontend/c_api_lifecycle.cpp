#include "libobjc3c_frontend/c_api.h"

#include <new>

#include "libobjc3c_frontend/objc3c_frontend_context_state.h"

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_context_t *
objc3c_frontend_c_context_create(void) {
  objc3c_frontend_c_context_t *context =
      new (std::nothrow) objc3c_frontend_c_context_t();
  if (context != nullptr) {
    context->last_error.clear();
  }
  return context;
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_c_context_destroy(
    objc3c_frontend_c_context_t *context) {
  delete context;
}
