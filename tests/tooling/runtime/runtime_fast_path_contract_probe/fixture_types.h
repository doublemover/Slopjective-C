#pragma once

#include <string>

#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" int callImplicit(void);
extern "C" int callExplicit(void);
extern "C" int callMixed(void);

namespace objc3c {
namespace tooling {
namespace runtime_fast_path_contract_probe {

constexpr int kProbeClassId = 1024;
constexpr const char *kDynamicSelector = "dynamicEscape";
constexpr const char *kStrictErrorSelector = "missingDispatch:";

struct MethodCacheStateObservation {
  int status = -1;
  objc3_runtime_method_cache_state_snapshot state{};
  std::string last_selector;
};

struct MethodCacheEntryObservation {
  int status = -1;
  objc3_runtime_method_cache_entry_snapshot entry{};
};

struct ProbeRun {
  MethodCacheStateObservation baseline;
  MethodCacheStateObservation direct;
  MethodCacheStateObservation mixed_first;
  MethodCacheStateObservation mixed_second;
  MethodCacheStateObservation strict_error_first;
  MethodCacheStateObservation strict_error_second;
  MethodCacheEntryObservation dynamic_entry;
  MethodCacheEntryObservation strict_error_entry;
  int implicit_value = 0;
  int explicit_value = 0;
  int mixed_first_value = 0;
  int mixed_second_value = 0;
  int strict_error_expected = 0;
  int strict_error_first_value = 0;
  int strict_error_second_value = 0;
};

inline std::string CopyRuntimeString(const char *value) {
  return value != nullptr ? value : "";
}

} // namespace runtime_fast_path_contract_probe
} // namespace tooling
} // namespace objc3c
