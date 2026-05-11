#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::class_realization_runtime {

struct MethodCacheStateObservation {
  objc3_runtime_method_cache_state_snapshot state{};
  std::string selector;
  std::string class_name;
  std::string owner;
};

struct MethodCacheEntryObservation {
  objc3_runtime_method_cache_entry_snapshot entry{};
  std::string selector;
  std::string class_name;
  std::string owner;
};

struct RuntimeRegistryObservation {
  objc3_runtime_registration_state_snapshot registration_state{};
  objc3_runtime_selector_lookup_table_state_snapshot selector_table_state{};
  std::string registration_module;
  std::string registration_identity;
  std::string registration_rejected_module;
  std::string registration_rejected_identity;
  std::string selector_table_last;
};

struct ClassRealizationValues {
  int inherited_value = 0;
  int category_value = 0;
  int class_value = 0;
  int known_class_value = 0;
  int protocol_strict_error = 0;
  int protocol_strict_error_cached = 0;
  int protocol_strict_error_expected = 0;
};

struct ClassRealizationProbeRun {
  ClassRealizationValues values;
  RuntimeRegistryObservation registry;
  MethodCacheStateObservation inherited_state;
  MethodCacheStateObservation category_state;
  MethodCacheStateObservation class_state;
  MethodCacheStateObservation known_class_state;
  MethodCacheStateObservation protocol_strict_error_state;
  MethodCacheStateObservation protocol_strict_error_cached_state;
  MethodCacheEntryObservation inherited_entry;
  MethodCacheEntryObservation category_entry;
  MethodCacheEntryObservation known_class_entry;
  MethodCacheEntryObservation protocol_strict_error_entry;
};

}  // namespace objc3c::runtime::probe::class_realization_runtime
