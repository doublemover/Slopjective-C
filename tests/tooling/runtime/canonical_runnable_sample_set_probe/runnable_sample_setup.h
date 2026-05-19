#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_RUNNABLE_SAMPLE_SETUP_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_RUNNABLE_SAMPLE_SETUP_H_

#include "sample_fixture_definitions.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::canonical_runnable_sample_set {

inline void StabilizeRealizedClassObservation(
    RealizedClassObservation &observation) {
  ::objc3c::runtime::probe::StabilizeRealizedClassEntry(
      observation.entry, observation.module, observation.identity,
      observation.class_name, observation.class_owner,
      observation.metaclass_owner, observation.super_class,
      observation.super_metaclass, observation.category_owner,
      observation.category_name);
}

inline void StabilizeRunnableSampleFixture(RunnableSampleFixture &fixture) {
  StabilizeRealizedClassObservation(fixture.widget_entry);
}

inline void CaptureRunnableSampleFixture(RunnableSampleFixture &fixture) {
  fixture = RunnableSampleFixture{};
  (void)objc3_runtime_copy_realized_class_entry_for_testing(
      kWidgetClassName, &fixture.widget_entry.entry);
  StabilizeRunnableSampleFixture(fixture);
  fixture.widget_class_receiver =
      static_cast<int>(fixture.widget_entry.entry.base_identity + 2U);
}

}  // namespace objc3c::runtime::probe::canonical_runnable_sample_set

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_RUNNABLE_SAMPLE_SETUP_H_
