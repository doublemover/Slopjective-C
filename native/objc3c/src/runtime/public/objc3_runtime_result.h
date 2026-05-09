#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Public runtime result/status contract.
 *
 * These enums and structs are the exported C ABI for runtime registration and
 * checked i32 dispatch results. Status values are result classifications, not
 * language capability claims. The checked dispatch payload always carries the
 * canonical diagnostic code/message pair for non-OK statuses.
 */
typedef enum objc3_runtime_registration_status_code {
  OBJC3_RUNTIME_REGISTRATION_STATUS_OK = 0,
  OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR = -1,
  OBJC3_RUNTIME_REGISTRATION_STATUS_DUPLICATE_TRANSLATION_UNIT_IDENTITY_KEY =
      -2,
  OBJC3_RUNTIME_REGISTRATION_STATUS_OUT_OF_ORDER_REGISTRATION = -3,
  OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_REGISTRATION_ROOTS = -4,
} objc3_runtime_registration_status_code;

typedef enum objc3_runtime_dispatch_status_code {
  OBJC3_RUNTIME_DISPATCH_STATUS_OK = 0,
  OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER = 1,
  OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR = -1,
  OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS = -2,
  OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH = -3,
  OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA = -4,
  OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE = -5,
  OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT = -6,
  OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT = -7
} objc3_runtime_dispatch_status_code;

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
