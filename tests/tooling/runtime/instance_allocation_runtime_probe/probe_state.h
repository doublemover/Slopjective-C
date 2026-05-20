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
  std::string lifecycle_failure_reason;
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

struct InstanceEntryObservation {
  objc3_runtime_instance_entry_snapshot entry{};
  std::string class_name;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
  std::string instance_isa_owner_identity;
  std::string class_object_isa_owner_identity;
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

struct AllocationFixture {
  int first_alloc = 0;
  int second_alloc = 0;
  int initialized_new = 0;
  objc3_runtime_dispatch_typed_result first_init_result{};
  objc3_runtime_dispatch_typed_result initialized_new_result{};
  objc3_runtime_dispatch_typed_result double_init_result{};
};

struct AllocationMutationResults {
  int set_base_count_first = 0;
  int base_count_value_first = 0;
  int base_count_value_second_before = 0;
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
  int set_base_count_second = 0;
  int base_count_value_first_after_second = 0;
  int base_count_value_second_after = 0;
  int set_value_second = 0;
  int value_result_first_after_second = 0;
  int value_result_second_after = 0;
};

struct AllocationInvariantSnapshots {
  MethodCacheEntryObservation count_entry;
  MethodCacheEntryObservation set_count_entry;
  MethodCacheEntryObservation base_count_entry;
  MethodCacheEntryObservation set_base_count_entry;
  RealizedGraphStateObservation graph_state;
  RealizedClassEntryObservation base_entry;
  RealizedClassEntryObservation widget_entry;
  InstanceEntryObservation first_instance;
  InstanceEntryObservation second_instance;
  InstanceEntryObservation initialized_new_instance;
  PropertyEntryObservation base_count_property;
  PropertyEntryObservation count_property;
  PropertyEntryObservation value_property;
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
