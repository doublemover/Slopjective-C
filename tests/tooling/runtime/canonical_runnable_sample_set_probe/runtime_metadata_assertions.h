#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_RUNTIME_METADATA_ASSERTIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_RUNTIME_METADATA_ASSERTIONS_H_

#include "sample_fixture_definitions.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c::runtime::probe::canonical_runnable_sample_set {

inline void StabilizeConformanceQueryObservation(
    ConformanceQueryObservation &observation) {
  ::objc3c::runtime::probe::StabilizeConformanceQuery(
      observation.query, observation.class_name, observation.protocol_name,
      observation.protocol_owner, observation.attachment_owner);
}

inline void StabilizePropertyEntryObservation(
    PropertyEntryObservation &observation) {
  ::objc3c::runtime::probe::StabilizePropertyEntry(
      observation.entry, observation.queried_class, observation.resolved_class,
      observation.property_name, observation.declaration_owner,
      observation.export_owner, observation.getter_selector,
      observation.setter_selector, observation.effective_getter_selector,
      observation.effective_setter_selector, observation.ivar_binding,
      observation.synthesized_binding, observation.layout_symbol,
      observation.getter_owner, observation.setter_owner);
}

inline void CopyConformanceQuery(const char *protocol_name,
                                 ConformanceQueryObservation &observation) {
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      kWidgetClassName, protocol_name, &observation.query);
}

inline void CopyPropertyEntry(const char *property_name,
                              PropertyEntryObservation &observation) {
  (void)objc3_runtime_copy_property_entry_for_testing(
      kWidgetClassName, property_name, &observation.entry);
}

inline RunnableSampleAssertions CaptureRunnableSampleAssertions() {
  RunnableSampleAssertions assertions;
  CopyConformanceQuery(kWorkerProtocolName, assertions.worker_query);
  CopyConformanceQuery(kTracerProtocolName, assertions.tracer_query);
  StabilizeConformanceQueryObservation(assertions.worker_query);
  StabilizeConformanceQueryObservation(assertions.tracer_query);

  CopyPropertyEntry(kCountPropertyName, assertions.count_property);
  CopyPropertyEntry(kValuePropertyName, assertions.value_property);
  CopyPropertyEntry(kTokenPropertyName, assertions.token_property);
  StabilizePropertyEntryObservation(assertions.count_property);
  StabilizePropertyEntryObservation(assertions.value_property);
  StabilizePropertyEntryObservation(assertions.token_property);
  return assertions;
}

}  // namespace objc3c::runtime::probe::canonical_runnable_sample_set

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_RUNTIME_METADATA_ASSERTIONS_H_
