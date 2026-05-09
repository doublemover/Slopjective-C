#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime {

struct EmittedPropertyDescriptor;
struct RealizedPropertyAccessor;

void PopulateRuntimePropertyEntryDescriptorSnapshotFields(
    const RealizedPropertyAccessor &accessor,
    const EmittedPropertyDescriptor &descriptor,
    objc3_runtime_property_entry_snapshot &snapshot);

}  // namespace objc3c::runtime
