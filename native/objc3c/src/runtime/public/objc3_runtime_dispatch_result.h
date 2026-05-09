#pragma once

#include "runtime/public/objc3_runtime_dispatch_status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_runtime_dispatch_i32_result {
  /*
   * OK means value is populated. Any other status means value is zeroed.
   */
  objc3_runtime_dispatch_status_code status_code;
  int value;
  /*
   * Runtime-owned diagnostic string literals. They remain valid for the process
   * lifetime and must not be freed or mutated by callers.
   */
  const char *diagnostic_code;
  const char *diagnostic_message;
  /*
   * Runtime-owned provenance fields for the diagnostic and fail-closed
   * ownership model. fallback_path_allowed is a fixed runtime contract flag,
   * not a compatibility escape hatch.
   */
  const char *diagnostic_owner_model;
  const char *fail_closed_ownership_model;
  int fallback_path_allowed;
} objc3_runtime_dispatch_i32_result;

#ifdef __cplusplus
}
#endif
