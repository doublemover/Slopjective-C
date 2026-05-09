#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime {

struct RuntimeContinuationState;

void ResetRuntimeAsyncContinuationStateSnapshot(
    objc3_runtime_async_continuation_state_snapshot &snapshot);
void PopulateRuntimeAsyncContinuationStateSnapshot(
    const RuntimeContinuationState &state,
    objc3_runtime_async_continuation_state_snapshot &snapshot);

}  // namespace objc3c::runtime
