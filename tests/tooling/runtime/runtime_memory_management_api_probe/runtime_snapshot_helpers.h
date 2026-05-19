#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_RUNTIME_SNAPSHOT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_RUNTIME_SNAPSHOT_HELPERS_H_

#include "probe_state.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::runtime_memory_management_api {

inline void CaptureRegistrationState(
    objc3_runtime_registration_state_snapshot &registration_state) {
  (void)objc3_runtime_copy_registration_state_for_testing(&registration_state);
}

inline void CaptureRealizedClassGraph(RealizedClassGraphCapture &capture) {
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &capture.snapshot);
  ::objc3c::runtime::probe::StabilizeRealizedClassGraph(
      capture.snapshot, capture.class_name_storage);
}

}  // namespace objc3c::runtime::probe::runtime_memory_management_api

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_RUNTIME_SNAPSHOT_HELPERS_H_
