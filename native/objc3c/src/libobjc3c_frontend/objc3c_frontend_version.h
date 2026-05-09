#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_VERSION_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_VERSION_H_

#include <stdint.h>

#ifndef OBJC3C_FRONTEND_API
#if defined(_WIN32)
#if defined(OBJC3C_FRONTEND_BUILD_DLL)
#define OBJC3C_FRONTEND_API __declspec(dllexport)
#elif defined(OBJC3C_FRONTEND_USE_DLL)
#define OBJC3C_FRONTEND_API __declspec(dllimport)
#else
#define OBJC3C_FRONTEND_API
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define OBJC3C_FRONTEND_API __attribute__((visibility("default")))
#else
#define OBJC3C_FRONTEND_API
#endif
#endif

#define OBJC3C_FRONTEND_VERSION_MAJOR 0u
#define OBJC3C_FRONTEND_VERSION_MINOR 1u
#define OBJC3C_FRONTEND_VERSION_PATCH 0u

#define OBJC3C_FRONTEND_ABI_VERSION 2u

#define OBJC3C_FRONTEND_VERSION_STRING "0.1.0"

#define OBJC3C_FRONTEND_VERSION_ENCODE(major, minor, patch) \
  (((uint32_t)(major) << 22) | ((uint32_t)(minor) << 12) | (uint32_t)(patch))

#define OBJC3C_FRONTEND_VERSION_VALUE                                                \
  OBJC3C_FRONTEND_VERSION_ENCODE(OBJC3C_FRONTEND_VERSION_MAJOR,                     \
                                 OBJC3C_FRONTEND_VERSION_MINOR,                      \
                                 OBJC3C_FRONTEND_VERSION_PATCH)

typedef struct objc3c_frontend_version {
  uint16_t major;
  uint16_t minor;
  uint16_t patch;
  uint16_t reserved;
  uint32_t abi_version;
} objc3c_frontend_version_t;

#define OBJC3C_FRONTEND_VERSION_INIT               \
  {                                                \
    (uint16_t)OBJC3C_FRONTEND_VERSION_MAJOR,       \
    (uint16_t)OBJC3C_FRONTEND_VERSION_MINOR,       \
    (uint16_t)OBJC3C_FRONTEND_VERSION_PATCH,       \
    (uint16_t)0u,                                  \
    (uint32_t)OBJC3C_FRONTEND_ABI_VERSION          \
  }

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Hard-cutover ABI gate. Returns non-zero only for OBJC3C_FRONTEND_ABI_VERSION.
 */
OBJC3C_FRONTEND_API uint8_t objc3c_frontend_is_exact_abi_version(
    uint32_t requested_abi_version);
OBJC3C_FRONTEND_API uint32_t objc3c_frontend_abi_version(void);
OBJC3C_FRONTEND_API objc3c_frontend_version_t objc3c_frontend_version(void);
/* Returns static read-only version storage; callers must not release it. */
OBJC3C_FRONTEND_API const char *objc3c_frontend_version_string(void);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_VERSION_H_
