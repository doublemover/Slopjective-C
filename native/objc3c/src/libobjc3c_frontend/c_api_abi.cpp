#include "libobjc3c_frontend/c_api.h"

#include <type_traits>

static_assert(OBJC3C_FRONTEND_C_API_ABI_VERSION ==
                  OBJC3C_FRONTEND_ABI_VERSION,
              "unexpected c api wrapper abi version");
static_assert(std::is_same_v<objc3c_frontend_c_context_t,
                             objc3c_frontend_context_t>,
              "context ABI mirror mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t,
                             objc3c_frontend_compile_options_t>,
              "compile options ABI mirror mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_compile_result_t,
                             objc3c_frontend_compile_result_t>,
              "compile result ABI mirror mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_artifact_kind_t,
                             objc3c_frontend_artifact_kind_t>,
              "artifact kind ABI mirror mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_string_t,
                             objc3c_frontend_string_t>,
              "string ABI mirror mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_string_view_t,
                             objc3c_frontend_string_view_t>,
              "string view ABI mirror mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_stage_summary_t,
                             objc3c_frontend_stage_summary_t>,
              "stage summary ABI mirror mismatch");
static_assert(std::is_same_v<objc3c_frontend_c_version_t,
                             objc3c_frontend_version_t>,
              "version ABI mirror mismatch");
static_assert(OBJC3C_FRONTEND_C_API_OWNER_RESULT_LIFECYCLE == 1,
              "result lifecycle owner id drift");
static_assert(OBJC3C_FRONTEND_C_API_OWNER_PUBLIC_PRIVATE_PARTITION == 8,
              "public/private partition owner id drift");
static_assert(OBJC3C_FRONTEND_C_API_POLICY_CALLER_OWNS_RESULT_STORAGE == 1,
              "result storage policy id drift");
static_assert(OBJC3C_FRONTEND_C_API_POLICY_C_NAMES_OWN_PACKAGE_SURFACE == 8,
              "public C package surface policy id drift");

extern "C" OBJC3C_FRONTEND_API uint32_t
objc3c_frontend_c_api_abi_version(void) {
  return OBJC3C_FRONTEND_C_API_ABI_VERSION;
}

extern "C" OBJC3C_FRONTEND_API uint8_t
objc3c_frontend_c_is_abi_compatible(uint32_t requested_abi_version) {
  return objc3c_frontend_is_abi_compatible(requested_abi_version);
}

extern "C" OBJC3C_FRONTEND_API uint32_t
objc3c_frontend_c_abi_version(void) {
  return objc3c_frontend_abi_version();
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_c_version_t
objc3c_frontend_c_version(void) {
  return objc3c_frontend_version();
}

extern "C" OBJC3C_FRONTEND_API const char *
objc3c_frontend_c_version_string(void) {
  return objc3c_frontend_version_string();
}
