#pragma once

#include "runtime/public/objc3_runtime_dispatch_status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_runtime_dispatch_i32_result {
  /*
   * OK means value is populated. Any other status means value is zeroed and the
   * diagnostic fields identify the rejected dispatch shape or runtime state.
   */
  objc3_runtime_dispatch_status_code status_code;
  int value;
  /*
   * Runtime-owned diagnostic string literals. They remain valid for the process
   * lifetime and must not be freed or mutated by callers.
   */
  const char *diagnostic_code;
  const char *diagnostic_message;
} objc3_runtime_dispatch_i32_result;

#ifdef __cplusplus
}
#endif
