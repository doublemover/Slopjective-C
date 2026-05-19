#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_FIXTURE_SETUP_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_FIXTURE_SETUP_H_

#include "probe_result.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::property_ivar_execution_matrix {

inline void StabilizeRealizedClassObservation(
    RealizedClassObservation &observation) {
  ::objc3c::runtime::probe::StabilizeRealizedClassEntry(
      observation.entry, observation.module, observation.identity,
      observation.class_name, observation.class_owner,
      observation.metaclass_owner, observation.super_class,
      observation.super_metaclass, observation.category_owner,
      observation.category_name);
}

inline void CaptureWidgetClassEntry(RealizedClassObservation &observation) {
  observation = RealizedClassObservation{};
  (void)objc3_runtime_copy_realized_class_entry_for_testing(
      kWidgetClassName, &observation.entry);
  StabilizeRealizedClassObservation(observation);
}

inline WidgetFixture SetUpWidgetFixture() {
  WidgetFixture fixture;
  fixture.widget_instance = objc3_runtime_dispatch_i32(
      kWidgetClassReceiver, kAllocSelector, 0, 0, 0, 0);
  return fixture;
}

inline void CaptureWidgetFixtureEntry(WidgetFixture &fixture) {
  fixture.widget_entry = RealizedClassObservation{};
  CaptureWidgetClassEntry(fixture.widget_entry);
}

}  // namespace objc3c::runtime::probe::property_ivar_execution_matrix

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_FIXTURE_SETUP_H_
