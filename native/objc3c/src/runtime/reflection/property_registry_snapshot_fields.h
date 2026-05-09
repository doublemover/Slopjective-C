#pragma once

#include "runtime/classes/runtime_object_snapshot_contracts.h"

namespace objc3c::runtime {

struct RuntimeState;

void ResetRuntimePropertyRegistryStateSnapshot(
    objc3_runtime_property_registry_state_snapshot &snapshot);
void PopulateRuntimePropertyRegistryStateSnapshotUnlocked(
    const RuntimeState &state,
    objc3_runtime_property_registry_state_snapshot &snapshot);

}  // namespace objc3c::runtime
