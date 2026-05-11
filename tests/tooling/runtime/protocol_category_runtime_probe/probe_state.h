#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::protocol_category_runtime {

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
  std::string selector_table_last;
};

struct ProtocolCategoryValues {
  int instance_first = 0;
  int instance_second = 0;
  int class_self = 0;
  int known_class = 0;
  int strict_error_first = 0;
  int strict_error_second = 0;
  int strict_error_expected = 0;
};

struct ProtocolCategoryProbeRun {
  ProtocolCategoryValues values;
  RuntimeRegistryObservation registry;
  MethodCacheStateObservation instance_first_state;
  MethodCacheStateObservation instance_second_state;
  MethodCacheStateObservation class_self_state;
  MethodCacheStateObservation known_class_state;
  MethodCacheStateObservation strict_error_first_state;
  MethodCacheStateObservation strict_error_second_state;
  MethodCacheEntryObservation instance_entry;
  MethodCacheEntryObservation class_entry;
  MethodCacheEntryObservation strict_error_entry;
};

}  // namespace objc3c::runtime::probe::protocol_category_runtime
