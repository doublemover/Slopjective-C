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
