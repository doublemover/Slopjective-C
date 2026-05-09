#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime {

struct RuntimeState;

void ResetRuntimePropertyRegistryStateSnapshot(
    objc3_runtime_property_registry_state_snapshot &snapshot);
void PopulateRuntimePropertyRegistryStateSnapshotUnlocked(
    const RuntimeState &state,
    objc3_runtime_property_registry_state_snapshot &snapshot);

}  // namespace objc3c::runtime
