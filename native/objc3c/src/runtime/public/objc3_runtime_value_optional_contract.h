#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int64_t objc3_runtime_value_optional_i64;

typedef enum objc3_runtime_value_optional_contract_version_i32 {
  OBJC3_RUNTIME_VALUE_OPTIONAL_CONTRACT_V1 = 1,
} objc3_runtime_value_optional_contract_version_i32;

objc3_runtime_value_optional_i64 objc3_runtime_optional_absent_i64(void);
objc3_runtime_value_optional_i64 objc3_runtime_optional_absent_bool(void);
objc3_runtime_value_optional_i64 objc3_runtime_optional_present_i32(
    int payload);
objc3_runtime_value_optional_i64 objc3_runtime_optional_present_bool(
    bool payload);
int objc3_runtime_optional_has_value_i32(
    objc3_runtime_value_optional_i64 value);
bool objc3_runtime_optional_has_value_bool(
    objc3_runtime_value_optional_i64 value);
int objc3_runtime_optional_payload_or_i32(
    objc3_runtime_value_optional_i64 value,
    int fallback);
bool objc3_runtime_optional_payload_or_bool(
    objc3_runtime_value_optional_i64 value,
    bool fallback);
int objc3_runtime_optional_unwrap_i32(
    objc3_runtime_value_optional_i64 value);
bool objc3_runtime_optional_unwrap_bool(
    objc3_runtime_value_optional_i64 value);

#ifdef __cplusplus
}
#endif
