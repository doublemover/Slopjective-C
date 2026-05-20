#pragma once

#include "runtime/dispatch/dispatch_target_resolution.h"
#include "runtime/dispatch/typed_dispatch_result.h"

namespace objc3c::runtime {

struct RuntimeState;

RuntimeTypedDispatchResult ExecuteResolvedRuntimeDispatchTargetStrict(
    RuntimeState &state, int receiver,
    const RuntimeDispatchTarget &dispatch_target, int a0, int a1, int a2,
    int a3);

}  // namespace objc3c::runtime
