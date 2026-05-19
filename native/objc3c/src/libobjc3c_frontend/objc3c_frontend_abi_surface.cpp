#include "libobjc3c_frontend/objc3c_frontend_version.h"

extern "C" OBJC3C_FRONTEND_API uint8_t objc3c_frontend_is_exact_abi_version(
    uint32_t requested_abi_version) {
  return requested_abi_version == OBJC3C_FRONTEND_ABI_VERSION ? 1u : 0u;
}

extern "C" OBJC3C_FRONTEND_API uint32_t objc3c_frontend_abi_version(void) {
  return OBJC3C_FRONTEND_ABI_VERSION;
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_version_t
objc3c_frontend_version(void) {
  const objc3c_frontend_version_t version = OBJC3C_FRONTEND_VERSION_INIT;
  return version;
}

extern "C" OBJC3C_FRONTEND_API const char *objc3c_frontend_version_string(void) {
  return OBJC3C_FRONTEND_VERSION_STRING;
}
