#pragma once

#include "probe_result.h"
#include "property_metadata_fixtures.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::runtime_property_metadata_reflection {

inline void StabilizeRealizedClassObservation(
    RealizedClassObservation &observation) {
  ::objc3c::runtime::probe::StabilizeRealizedClassEntry(
      observation.entry, observation.module, observation.identity,
      observation.class_name, observation.class_owner,
      observation.metaclass_owner, observation.super_class,
      observation.super_metaclass, observation.category_owner,
      observation.category_name);
}

inline void CaptureRuntimeReflectionFixture(RuntimeReflectionFixture &fixture) {
  (void)objc3_runtime_copy_realized_class_entry_for_testing(
      kWidgetClassName, &fixture.widget_entry.entry);
  StabilizeRealizedClassObservation(fixture.widget_entry);
}

}  // namespace objc3c::runtime::probe::runtime_property_metadata_reflection
