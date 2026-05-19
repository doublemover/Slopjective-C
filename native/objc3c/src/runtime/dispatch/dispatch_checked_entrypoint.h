#pragma once

#include "runtime/public/objc3_runtime_dispatch_result.h"

namespace objc3c::runtime {

objc3_runtime_dispatch_i32_result ExecuteRuntimeDispatchI32Checked(
    int receiver, const char *selector, int a0, int a1, int a2, int a3);

}  // namespace objc3c::runtime
