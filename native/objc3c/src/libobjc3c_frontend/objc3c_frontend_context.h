#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_CONTEXT_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_CONTEXT_H_

#include "objc3c_frontend_version.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3c_frontend_context objc3c_frontend_context_t;

OBJC3C_FRONTEND_API objc3c_frontend_context_t *objc3c_frontend_context_create(void);
OBJC3C_FRONTEND_API void objc3c_frontend_context_destroy(objc3c_frontend_context_t *context);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_CONTEXT_H_
