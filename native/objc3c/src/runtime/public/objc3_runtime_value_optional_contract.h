#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int64_t objc3_runtime_value_optional_i64;

typedef enum objc3_runtime_value_optional_contract_version_i32 {
  OBJC3_RUNTIME_VALUE_OPTIONAL_CONTRACT_V1 = 1,
} objc3_runtime_value_optional_contract_version_i32;

objc3_runtime_value_optional_i64 objc3_runtime_optional_absent_i64(void);
objc3_runtime_value_optional_i64 objc3_runtime_optional_present_i32(
    int payload);
int objc3_runtime_optional_has_value_i32(
    objc3_runtime_value_optional_i64 value);
int objc3_runtime_optional_payload_or_i32(
    objc3_runtime_value_optional_i64 value,
    int fallback);
int objc3_runtime_optional_unwrap_i32(
    objc3_runtime_value_optional_i64 value);

#ifdef __cplusplus
}
#endif
