#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime {

struct RuntimeState;

void CopyRuntimePropertyEntrySnapshotForQueryUnlocked(
    RuntimeState &state,
    const char *class_name,
    const char *property_name,
    objc3_runtime_property_entry_snapshot &snapshot);

}  // namespace objc3c::runtime
