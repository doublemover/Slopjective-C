#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c {
namespace tooling {
namespace method_binding_probe {

struct RegistrationObservation {
  int status = -1;
  objc3_runtime_registration_state_snapshot state{};
  std::string module_storage;
  std::string identity_storage;
};

struct SelectorTableObservation {
  int status = -1;
  objc3_runtime_selector_lookup_table_state_snapshot state{};
  std::string last_storage;
};

struct MethodCacheStateObservation {
  int status = -1;
  objc3_runtime_method_cache_state_snapshot state{};
  std::string selector_storage;
  std::string class_storage;
  std::string owner_storage;
};

struct MethodCacheEntryObservation {
  int status = -1;
  objc3_runtime_method_cache_entry_snapshot entry{};
  std::string selector_storage;
  std::string class_storage;
  std::string owner_storage;
};

struct MethodBindingDispatchValues {
  int instance_first = 0;
  int instance_second = 0;
  int class_value = 0;
  int known_class_value = 0;
  int category_value = 0;
};

struct MethodBindingProbeRun {
  RegistrationObservation registration;
  SelectorTableObservation selector_table;
  MethodBindingDispatchValues values;

  MethodCacheStateObservation instance_first_state;
  MethodCacheStateObservation instance_second_state;
  MethodCacheStateObservation class_state;
  MethodCacheStateObservation known_class_state;
  MethodCacheStateObservation category_state;

  MethodCacheEntryObservation instance_entry;
  MethodCacheEntryObservation class_entry;
  MethodCacheEntryObservation category_entry;
};

} // namespace method_binding_probe
} // namespace tooling
} // namespace objc3c
