#pragma once

#include "runtime/dispatch/dispatch_target_resolution.h"
#include "runtime/public/objc3_runtime_dispatch_result.h"

namespace objc3c::runtime {

struct RuntimeState;

objc3_runtime_dispatch_i32_result ExecuteResolvedRuntimeDispatchTargetStrict(
    RuntimeState &state, int receiver,
    const RuntimeDispatchTarget &dispatch_target, int a0, int a1, int a2,
    int a3);

}  // namespace objc3c::runtime
