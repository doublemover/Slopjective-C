#include "libobjc3c_frontend/c_api.h"

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_context_t *
objc3c_frontend_c_context_create(void) {
  return objc3c_frontend_context_create();
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_c_context_destroy(
    objc3c_frontend_c_context_t *context) {
  objc3c_frontend_context_destroy(context);
}
