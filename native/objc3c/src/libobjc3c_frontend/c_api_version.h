#ifndef OBJC3C_LIBOBJC3C_FRONTEND_C_API_VERSION_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_C_API_VERSION_H_

#include <stdint.h>

#include "c_api_types.h"

#define OBJC3C_FRONTEND_C_API_ABI_VERSION OBJC3C_FRONTEND_ABI_VERSION

#ifdef __cplusplus
extern "C" {
#endif

OBJC3C_FRONTEND_API uint32_t objc3c_frontend_c_api_abi_version(void);

/*
 * Hard-cutover ABI gate. Returns non-zero only for the exact public C ABI
 * version exposed by this library; no compatibility windows are honored.
 */
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_c_is_exact_abi_version(
    uint32_t requested_abi_version);
OBJC3C_FRONTEND_API uint32_t objc3c_frontend_c_abi_version(void);
OBJC3C_FRONTEND_API objc3c_frontend_c_version_t objc3c_frontend_c_version(void);
/* Returns static read-only version storage; callers must not release it. */
OBJC3C_FRONTEND_API const char *objc3c_frontend_c_version_string(void);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_C_API_VERSION_H_
