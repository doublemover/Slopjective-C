#pragma once

#include "runtime/classes/runtime_object_snapshot_contracts.h"

namespace objc3c::runtime {

struct EmittedPropertyDescriptor;
struct RealizedClassNode;
struct RealizedPropertyAccessor;

void PopulateRuntimePropertyEntryLayoutSnapshotFields(
    const RealizedClassNode &resolved_node,
    const RealizedPropertyAccessor &accessor,
    const EmittedPropertyDescriptor &descriptor,
    objc3_runtime_property_entry_snapshot &snapshot);

}  // namespace objc3c::runtime
