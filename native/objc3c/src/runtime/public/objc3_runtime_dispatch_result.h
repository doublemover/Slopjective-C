#pragma once

#include "runtime/public/objc3_runtime_dispatch_status.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OBJC3_RUNTIME_DISPATCH_I32_RESULT_ABI_VERSION 2u

typedef enum objc3_runtime_dispatch_return_kind_code {
  OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED = 0,
  OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32 = 1,
  OBJC3_RUNTIME_DISPATCH_RETURN_KIND_BOOL = 2,
  OBJC3_RUNTIME_DISPATCH_RETURN_KIND_VOID = 3,
  OBJC3_RUNTIME_DISPATCH_RETURN_KIND_OBJECT_REFERENCE = 4,
  OBJC3_RUNTIME_DISPATCH_RETURN_KIND_CLASS_REFERENCE = 5,
  OBJC3_RUNTIME_DISPATCH_RETURN_KIND_SELECTOR_REFERENCE = 6,
  OBJC3_RUNTIME_DISPATCH_RETURN_KIND_PROTOCOL_REFERENCE = 7
} objc3_runtime_dispatch_return_kind_code;

typedef struct objc3_runtime_dispatch_i32_result {
  /*
   * Versioned checked-dispatch envelope. result_size lets callers detect tail
   * fields when the ABI is widened again.
   */
  uint32_t abi_version;
  uint32_t result_size;
  /*
   * OK means value is populated. Any other status means value is zeroed.
   */
  objc3_runtime_dispatch_status_code status_code;
  objc3_runtime_dispatch_return_kind_code return_kind;
  int value;
  /*
   * Runtime-owned diagnostic string literals. They remain valid for the process
   * lifetime and must not be freed or mutated by callers.
   */
  const char *diagnostic_code;
  const char *diagnostic_message;
  const char *result_contract;
  /*
   * Runtime-owned provenance fields for the diagnostic and fail-closed
   * ownership model. retired_route_path_allowed is a fixed runtime contract flag,
   * not a compatibility escape hatch.
   */
  const char *diagnostic_owner_model;
  const char *fail_closed_ownership_model;
  int retired_route_path_allowed;
} objc3_runtime_dispatch_i32_result;

#ifdef __cplusplus
}
#endif
