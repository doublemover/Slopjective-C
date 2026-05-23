#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_SAMPLE_FIXTURE_DEFINITIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_SAMPLE_FIXTURE_DEFINITIONS_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::canonical_runnable_sample_set {

inline constexpr const char *kWidgetClassName = "Widget";
inline constexpr const char *kWorkerProtocolName = "Worker";
inline constexpr const char *kTracerProtocolName = "Tracer";
inline constexpr const char *kAllocSelector = "alloc";
inline constexpr const char *kInitSelector = "init";
inline constexpr const char *kTracedValueSelector = "tracedValue";
inline constexpr const char *kInheritedValueSelector = "inheritedValue";
inline constexpr const char *kClassValueSelector = "classValue";
inline constexpr const char *kSharedSelector = "shared";
inline constexpr const char *kSetCountSelector = "setCount:";
inline constexpr const char *kCountSelector = "count";
inline constexpr const char *kSetEnabledSelector = "setEnabled:";
inline constexpr const char *kEnabledSelector = "enabled";
inline constexpr const char *kSetCurrentValueSelector = "setCurrentValue:";
inline constexpr const char *kCurrentValueSelector = "currentValue";
inline constexpr const char *kTokenValueSelector = "tokenValue";
inline constexpr const char *kCountPropertyName = "count";
inline constexpr const char *kValuePropertyName = "value";
inline constexpr const char *kTokenPropertyName = "token";

struct RealizedClassObservation {
  objc3_runtime_realized_class_entry_snapshot entry{};
  std::string module;
  std::string identity;
  std::string class_name;
  std::string class_owner;
  std::string metaclass_owner;
  std::string super_class;
  std::string super_metaclass;
  std::string category_owner;
  std::string category_name;
};

struct ConformanceQueryObservation {
  objc3_runtime_protocol_conformance_query_snapshot query{};
  std::string class_name;
  std::string protocol_name;
  std::string protocol_owner;
  std::string attachment_owner;
  std::string matched_class_name;
  std::string matched_class_owner;
  std::string failure_reason;
  std::string existential_canonical_spelling;
  std::string object_representation;
  std::string conformance_owner_identity;
  std::string runtime_lookup_anchor;
  std::string witness_metadata_key;
  std::string requirement_resolution_policy;
  std::string unsupported_associated_type_diagnostic;
  std::string unsupported_dynamic_dispatch_diagnostic;
};

struct PropertyEntryObservation {
  objc3_runtime_property_entry_snapshot entry{};
  std::string queried_class;
  std::string resolved_class;
  std::string property_name;
  std::string declaration_owner;
  std::string export_owner;
  std::string getter_selector;
  std::string setter_selector;
  std::string effective_getter_selector;
  std::string effective_setter_selector;
  std::string ivar_binding;
  std::string synthesized_binding;
  std::string layout_symbol;
  std::string getter_owner;
  std::string setter_owner;
};

struct RunnableSampleFixture {
  RealizedClassObservation widget_entry;
  int widget_class_receiver = 0;
};

struct RunnableSampleExecution {
  int widget_instance = 0;
  int init_value = 0;
  int traced_value = 0;
  int inherited_value = 0;
  int class_value = 0;
  int shared_value = 0;
  int count_value = 0;
  int enabled_value = 0;
  int current_value = 0;
  int token_value = 0;
};

struct RunnableSampleAssertions {
  ConformanceQueryObservation worker_query;
  ConformanceQueryObservation tracer_query;
  PropertyEntryObservation count_property;
  PropertyEntryObservation value_property;
  PropertyEntryObservation token_property;
};

struct ProbeResult {
  RunnableSampleFixture fixture;
  RunnableSampleExecution execution;
  RunnableSampleAssertions assertions;
};

}  // namespace objc3c::runtime::probe::canonical_runnable_sample_set

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_SAMPLE_FIXTURE_DEFINITIONS_H_
