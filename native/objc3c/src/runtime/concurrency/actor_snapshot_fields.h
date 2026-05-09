#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime {

struct RuntimeActorState;

void ResetRuntimeActorRuntimeStateSnapshot(
    objc3_runtime_actor_runtime_state_snapshot &snapshot);
void PopulateRuntimeActorRuntimeStateSnapshot(
    const RuntimeActorState &state,
    objc3_runtime_actor_runtime_state_snapshot &snapshot);

}  // namespace objc3c::runtime
