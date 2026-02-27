#ifndef OBJC3C_LIBOBJC3C_FRONTEND_VERSION_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_VERSION_H_

#include <stdint.h>

#define OBJC3C_FRONTEND_VERSION_MAJOR 0u
#define OBJC3C_FRONTEND_VERSION_MINOR 1u
#define OBJC3C_FRONTEND_VERSION_PATCH 0u

#define OBJC3C_FRONTEND_ABI_VERSION 1u

/* Compatibility policy: SemVer; ABI breaks require major version bump. */
#define OBJC3C_FRONTEND_MIN_COMPATIBILITY_ABI_VERSION 1u
#define OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION OBJC3C_FRONTEND_ABI_VERSION
#define OBJC3C_FRONTEND_DEPRECATION_WINDOW_MAJOR 1u

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

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_VERSION_H_
