#pragma once

#include "runtime/classes/runtime_object_snapshot_contracts.h"

namespace objc3c::runtime {

struct RealizedClassNode;
struct RealizedPropertyAccessor;
struct RuntimeState;

void ResetRuntimePropertyEntrySnapshot(
    objc3_runtime_property_entry_snapshot &snapshot);
void PopulateRuntimePropertyEntrySnapshotUnlocked(
    const RuntimeState &state,
    const RealizedClassNode &resolved_node,
    const RealizedPropertyAccessor &accessor,
    bool inherited,
    objc3_runtime_property_entry_snapshot &snapshot);

}  // namespace objc3c::runtime
