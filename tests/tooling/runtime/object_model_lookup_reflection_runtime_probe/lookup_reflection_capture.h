#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_LOOKUP_REFLECTION_CAPTURE_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_LOOKUP_REFLECTION_CAPTURE_H_

#include "object_model_fixture_setup.h"
#include "probe_result.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::object_model_lookup_reflection_runtime {

inline void StabilizeAggregateQueryObservation(
    AggregateQueryObservation &observation) {
  ::objc3c::runtime::probe::StabilizeAggregateSnapshot(
      observation.snapshot, observation.queried_class,
      observation.resolved_class, observation.resolved_class_owner,
      observation.queried_property, observation.resolved_property_class,
      observation.resolved_property_owner, observation.queried_protocol_class,
      observation.queried_protocol, observation.matched_protocol_owner,
      observation.matched_attachment_owner);
}

inline void CaptureCountPropertyLookup(
    PropertyLookupObservation &observation) {
  (void)objc3_runtime_copy_property_entry_for_testing(
      kWidgetClassName, kCountPropertyName, &observation.snapshot);
}

inline void CaptureTracerConformanceLookup(
    ProtocolConformanceObservation &observation) {
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      kWidgetClassName, kTracerProtocolName, &observation.snapshot);
}

inline void CaptureAggregateLookupState(AggregateQueryObservation &aggregate) {
  (void)objc3_runtime_copy_object_model_query_state_for_testing(
      &aggregate.snapshot);
  StabilizeAggregateQueryObservation(aggregate);
}

inline void CaptureLookupReflectionState(
    LookupReflectionCapture &reflection) {
  CaptureCountPropertyLookup(reflection.count_property);
  CaptureTracerConformanceLookup(reflection.tracer_query);
  CaptureAggregateLookupState(reflection.aggregate);
}

}  // namespace objc3c::runtime::probe::object_model_lookup_reflection_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_LOOKUP_REFLECTION_CAPTURE_H_
