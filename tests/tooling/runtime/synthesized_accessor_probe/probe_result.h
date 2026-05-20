#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_PROBE_RESULT_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_PROBE_RESULT_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::synthesized_accessor {

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

struct AccessorSetup {
  RegistrationStateObservation registration_state;
  SelectorTableStateObservation selector_table_state;
};

struct AccessorActions {
  int widget_instance = 0;
  int set_count_result = 0;
  int count_value = 0;
  int set_enabled_result = 0;
  int enabled_value = 0;
  int set_value_result = 0;
  int value_result = 0;
};

struct AccessorAssertions {
  PropertyEntryObservation count_property;
  PropertyEntryObservation enabled_property;
  PropertyEntryObservation value_property;
  MethodCacheEntryObservation count_entry;
  MethodCacheEntryObservation set_count_entry;
  MethodCacheEntryObservation enabled_entry;
  MethodCacheEntryObservation set_enabled_entry;
  MethodCacheEntryObservation value_entry;
  MethodCacheEntryObservation set_value_entry;
};

struct ProbeResult {
  AccessorSetup setup;
  AccessorActions actions;
  AccessorAssertions assertions;
};

}  // namespace objc3c::runtime::probe::synthesized_accessor

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_PROBE_RESULT_H_
