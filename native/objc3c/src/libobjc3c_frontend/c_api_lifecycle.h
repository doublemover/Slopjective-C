#ifndef OBJC3C_LIBOBJC3C_FRONTEND_C_API_LIFECYCLE_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_C_API_LIFECYCLE_H_

#include "c_api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

OBJC3C_FRONTEND_API objc3c_frontend_c_context_t *
objc3c_frontend_c_context_create(void);
OBJC3C_FRONTEND_API void objc3c_frontend_c_context_destroy(
    objc3c_frontend_c_context_t *context);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_C_API_LIFECYCLE_H_
