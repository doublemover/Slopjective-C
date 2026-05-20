#pragma once

#include "runtime/public/objc3_runtime_dispatch_result.h"

namespace objc3c::runtime {

/*
 * Single runtime/public boundary for constructing checked dispatch results.
 */
objc3_runtime_dispatch_typed_result MakeRuntimeDispatchTypedResult(
    objc3_runtime_dispatch_status_code status_code, int value,
    objc3_runtime_dispatch_return_kind_code return_kind);
objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32Result(
    objc3_runtime_dispatch_status_code status_code, int value);
objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32TypedResult(
    objc3_runtime_dispatch_status_code status_code, int value,
    objc3_runtime_dispatch_return_kind_code return_kind);
objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32ResultFromTypedResult(
    const objc3_runtime_dispatch_typed_result &typed_result);

}  // namespace objc3c::runtime
