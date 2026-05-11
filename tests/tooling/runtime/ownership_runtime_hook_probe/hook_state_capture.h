#pragma once

#include "probe_state.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::ownership_runtime_hook {

inline void CaptureRealizedClassGraph(RealizedClassGraphCapture &capture) {
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &capture.snapshot);
  ::objc3c::runtime::probe::StabilizeRealizedClassGraph(
      capture.snapshot, capture.class_name_storage);
}

inline void CapturePropertyEntryOwnershipHook(
    const char *class_name,
    const char *property_name,
    PropertyEntryOwnershipCapture &capture) {
  (void)objc3_runtime_copy_property_entry_for_testing(class_name, property_name,
                                                      &capture.entry);
  ::objc3c::runtime::probe::StabilizePropertyEntry(
      capture.entry, capture.queried_class_storage,
      capture.resolved_class_storage, capture.property_name_storage,
      capture.declaration_owner_storage, capture.getter_owner_storage,
      capture.setter_owner_storage, capture.lifetime_storage,
      capture.hook_storage, capture.accessor_storage);
}

}  // namespace objc3c::runtime::probe::ownership_runtime_hook
