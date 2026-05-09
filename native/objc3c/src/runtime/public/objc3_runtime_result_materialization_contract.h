#pragma once

#include "runtime/public/objc3_runtime_dispatch_result.h"

namespace objc3c::runtime {

/*
 * Single runtime/public boundary for constructing checked i32 dispatch results.
 */
objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32Result(
    objc3_runtime_dispatch_status_code status_code, int value);

}  // namespace objc3c::runtime
