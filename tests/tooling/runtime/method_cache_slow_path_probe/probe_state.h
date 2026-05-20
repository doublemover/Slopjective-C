#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c {
namespace tooling {
namespace method_cache_slow_path_probe {

struct RegistrationObservation {
  objc3_runtime_registration_state_snapshot state{};
  std::string module_storage;
  std::string identity_storage;
};

struct SelectorTableObservation {
  objc3_runtime_selector_lookup_table_state_snapshot state{};
  std::string last_storage;
};

struct MethodCacheStateObservation {
  objc3_runtime_method_cache_state_snapshot state{};
  std::string selector_storage;
  std::string class_storage;
  std::string owner_storage;
};

struct MethodCacheEntryObservation {
  objc3_runtime_method_cache_entry_snapshot entry{};
  std::string selector_storage;
  std::string class_storage;
  std::string owner_storage;
};

struct SlowPathProbeRun {
  RegistrationObservation registration;
  SelectorTableObservation selector_table;

  int instance_first = 0;
  int instance_second = 0;
  int instance_after_stale = 0;
  int class_self = 0;
  int known_class = 0;
  int strict_error_first = 0;
  int strict_error_second = 0;
  int strict_error_expected = 0;
  int mutation_registration_status = 0;

  MethodCacheStateObservation instance_first_state;
  MethodCacheStateObservation instance_second_state;
  MethodCacheStateObservation instance_after_stale_state;
  MethodCacheStateObservation class_self_state;
  MethodCacheStateObservation known_class_state;
  MethodCacheStateObservation strict_error_first_state;
  MethodCacheStateObservation strict_error_second_state;

  MethodCacheEntryObservation instance_entry;
  MethodCacheEntryObservation instance_after_stale_entry;
  MethodCacheEntryObservation class_entry;
  MethodCacheEntryObservation strict_error_entry;

  RegistrationObservation registration_after_mutation;
};

} // namespace method_cache_slow_path_probe
} // namespace tooling
} // namespace objc3c
