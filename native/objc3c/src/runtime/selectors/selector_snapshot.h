#pragma once

#include "runtime/public/objc3_runtime_api.h"
#include "runtime/selectors/runtime_selector_snapshot_contracts.h"

namespace objc3c::runtime {

struct RuntimeState;

void CopySelectorLookupTableStateSnapshotUnlocked(
    const RuntimeState &state,
    objc3_runtime_selector_lookup_table_state_snapshot *snapshot);
void InitializeSelectorLookupEntrySnapshot(
    objc3_runtime_selector_lookup_entry_snapshot *snapshot);
void CopySelectorLookupEntrySnapshotUnlocked(
    const RuntimeState &state,
    const char *selector,
    objc3_runtime_selector_lookup_entry_snapshot *snapshot);

}  // namespace objc3c::runtime
