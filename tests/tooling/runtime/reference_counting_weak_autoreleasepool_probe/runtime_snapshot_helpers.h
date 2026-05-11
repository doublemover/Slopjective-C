#pragma once

#include "probe_state.h"

#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool {

inline void CaptureRealizedClassGraph(RealizedClassGraphCapture &capture) {
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &capture.snapshot);
  ::objc3c::runtime::probe::StabilizeRealizedClassGraph(
      capture.snapshot, capture.allocated_class_name_storage);
}

inline void CaptureMemoryManagementState(MemoryManagementCapture &capture) {
  (void)objc3_runtime_copy_memory_management_state_for_testing(
      &capture.snapshot);
}

inline void CaptureWeakValuePropertyEntry(WeakPropertyEntryCapture &capture) {
  (void)objc3_runtime_copy_property_entry_for_testing("Box", "weakValue",
                                                      &capture.snapshot);
  ::objc3c::runtime::probe::StabilizePropertyEntry(
      capture.snapshot, capture.queried_class_name_storage,
      capture.resolved_class_name_storage, capture.property_name_storage,
      capture.declaration_owner_storage, capture.ownership_lifetime_storage,
      capture.ownership_runtime_hook_storage,
      capture.accessor_ownership_storage);
}

}  // namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool
