#pragma once

#include "runtime/public/objc3_runtime_dispatch_status.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OBJC3_RUNTIME_DISPATCH_I32_RESULT_ABI_VERSION 3u
#define OBJC3_RUNTIME_DISPATCH_TYPED_RESULT_ABI_VERSION 2u

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

typedef struct objc3_runtime_dispatch_typed_result {
  /*
   * Versioned typed-dispatch envelope. result_size lets callers detect tail
   * fields when the ABI is widened again.
   */
  uint32_t abi_version;
  uint32_t result_size;
  /*
   * OK means exactly one value field is populated according to return_kind,
   * except void which is represented by return_kind alone. Any other status
   * means every value field is zeroed.
   */
  objc3_runtime_dispatch_status_code status_code;
  objc3_runtime_dispatch_return_kind_code return_kind;
  const char *return_kind_name;
  int i32_value;
  int bool_value;
  int object_reference;
  int class_reference;
  int selector_reference;
  int protocol_reference;
  /*
   * Runtime-owned diagnostic string literals. They remain valid for the process
   * lifetime and must not be freed or mutated by callers.
   */
  const char *diagnostic_code;
  const char *diagnostic_message;
  const char *result_contract;
  /*
   * Runtime-owned provenance fields for the diagnostic and fail-closed
   * ownership model.
   */
  const char *diagnostic_owner_model;
  const char *fail_closed_ownership_model;
} objc3_runtime_dispatch_typed_result;

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
   * ownership model.
   */
  const char *diagnostic_owner_model;
  const char *fail_closed_ownership_model;
} objc3_runtime_dispatch_i32_result;

#ifdef __cplusplus
}
#endif
