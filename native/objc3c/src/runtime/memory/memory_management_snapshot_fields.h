#pragma once

#include "runtime/memory/memory_management_snapshot_contracts.h"

namespace objc3c::runtime {

struct RuntimeState;

void ResetRuntimeMemoryManagementStateSnapshot(
    objc3_runtime_memory_management_state_snapshot &snapshot);
void PopulateRuntimeAutoreleaseSnapshotFields(
    objc3_runtime_memory_management_state_snapshot &snapshot);
void PopulateRuntimeMemoryManagementSnapshotFieldsUnlocked(
    const RuntimeState &state,
    objc3_runtime_memory_management_state_snapshot &snapshot);

}  // namespace objc3c::runtime
