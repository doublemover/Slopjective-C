#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

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
