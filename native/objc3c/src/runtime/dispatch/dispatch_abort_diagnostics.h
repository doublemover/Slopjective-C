#pragma once

#include "runtime/public/objc3_runtime_dispatch_result.h"

namespace objc3c::runtime {

[[noreturn]] void AbortRuntimeDispatchFailure(
    const objc3_runtime_dispatch_i32_result &result);

}  // namespace objc3c::runtime
