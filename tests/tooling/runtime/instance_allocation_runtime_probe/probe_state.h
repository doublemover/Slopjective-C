#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_PROBE_STATE_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_PROBE_STATE_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::instance_allocation_runtime {

struct RegistrationStateObservation {
  objc3_runtime_registration_state_snapshot state{};
  std::string module;
  std::string identity;
};

struct SelectorTableStateObservation {
  objc3_runtime_selector_lookup_table_state_snapshot state{};
  std::string last_selector;
};

struct MethodCacheEntryObservation {
  objc3_runtime_method_cache_entry_snapshot entry{};
  std::string selector;
  std::string class_name;
  std::string owner;
};

struct RealizedGraphStateObservation {
  objc3_runtime_realized_class_graph_state_snapshot state{};
  std::string class_name;
  std::string owner;
  std::string metaclass;
  std::string category_owner;
  std::string category_name;
  std::string allocated_class;
};

struct RealizedClassEntryObservation {
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

struct AllocationFixture {
  int first_alloc = 0;
  int second_alloc = 0;
};

struct AllocationMutationResults {
  int set_count_first = 0;
  int count_value_first = 0;
  int count_value_second_before = 0;
  int set_enabled_first = 0;
  int enabled_value_first = 0;
  int enabled_value_second = 0;
  int set_value_first = 0;
  int value_result_first = 0;
  int value_result_second_before = 0;
  int set_count_second = 0;
  int count_value_first_after_second = 0;
  int count_value_second_after = 0;
  int set_value_second = 0;
  int value_result_first_after_second = 0;
  int value_result_second_after = 0;
};

struct AllocationInvariantSnapshots {
  MethodCacheEntryObservation count_entry;
  MethodCacheEntryObservation set_count_entry;
  RealizedGraphStateObservation graph_state;
  RealizedClassEntryObservation widget_entry;
};

struct ProbeRun {
  RegistrationStateObservation registration_state;
  SelectorTableStateObservation selector_table_state;
  AllocationFixture fixture;
  AllocationMutationResults mutations;
  AllocationInvariantSnapshots invariants;
};

}  // namespace objc3c::runtime::probe::instance_allocation_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_PROBE_STATE_H_
