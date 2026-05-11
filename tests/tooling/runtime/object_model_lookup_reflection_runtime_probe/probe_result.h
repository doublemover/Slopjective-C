#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_PROBE_RESULT_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_PROBE_RESULT_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::object_model_lookup_reflection_runtime {

struct RealizedClassObservation {
  objc3_runtime_realized_class_entry_snapshot snapshot{};
};

struct ObjectModelFixture {
  RealizedClassObservation widget_class;
  int widget_class_receiver = 0;
  int widget_instance = 0;
  int initialized_widget = 0;
};

struct RuntimeDispatchAssertions {
  int traced_value = 0;
  int count_value = 0;
};

struct PropertyLookupObservation {
  objc3_runtime_property_entry_snapshot snapshot{};
};

struct ProtocolConformanceObservation {
  objc3_runtime_protocol_conformance_query_snapshot snapshot{};
};

struct AggregateQueryObservation {
  objc3_runtime_object_model_query_state_snapshot snapshot{};
  std::string queried_class;
  std::string resolved_class;
  std::string resolved_class_owner;
  std::string queried_property;
  std::string resolved_property_class;
  std::string resolved_property_owner;
  std::string queried_protocol_class;
  std::string queried_protocol;
  std::string matched_protocol_owner;
  std::string matched_attachment_owner;
};

struct LookupReflectionCapture {
  PropertyLookupObservation count_property;
  ProtocolConformanceObservation tracer_query;
  AggregateQueryObservation aggregate;
};

struct ProbeResult {
  ObjectModelFixture fixture;
  RuntimeDispatchAssertions dispatch;
  LookupReflectionCapture reflection;
};

}  // namespace objc3c::runtime::probe::object_model_lookup_reflection_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_PROBE_RESULT_H_
