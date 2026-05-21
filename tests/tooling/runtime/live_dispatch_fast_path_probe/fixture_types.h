#pragma once

#include <string>

#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" int callImplicit(void);
extern "C" int callExplicit(void);
extern "C" int callMixed(void);

namespace objc3c {
namespace tooling {
namespace live_dispatch_fast_path_probe {

constexpr int kProbeClassId = 1024;
constexpr const char *kDynamicSelector = "dynamicEscape";
constexpr const char *kExplicitSelector = "explicitDirect";
constexpr const char *kStrictErrorSelector = "missingDispatch:";

struct MethodCacheStateObservation {
  int status = -1;
  objc3_runtime_method_cache_state_snapshot state{};
  std::string last_selector;
  std::string last_fast_path_reason;
};

struct DispatchStateObservation {
  int status = -1;
  objc3_runtime_dispatch_state_snapshot state{};
  std::string last_selector;
  std::string last_fast_path_reason;
  std::string last_path;
  std::string last_implementation_kind;
  std::string last_resolved_class_name;
};

struct MethodCacheEntryObservation {
  int status = -1;
  objc3_runtime_method_cache_entry_snapshot entry{};
  std::string selector;
  std::string fast_path_reason;
};

struct CacheAwareDispatchObservation {
  int status = -1;
  objc3_runtime_cache_aware_dispatch_record_snapshot record{};
  std::string selector;
  std::string source_path;
  std::string dispatch_path;
  std::string implementation_kind;
  std::string diagnostic_code;
};

struct ProbeRun {
  MethodCacheStateObservation baseline;
  MethodCacheStateObservation direct;
  MethodCacheStateObservation mixed_first;
  MethodCacheStateObservation mixed_second;
  MethodCacheStateObservation strict_error_first;
  MethodCacheStateObservation strict_error_second;
  DispatchStateObservation mixed_first_dispatch;
  DispatchStateObservation mixed_second_dispatch;
  DispatchStateObservation strict_error_first_dispatch;
  DispatchStateObservation strict_error_second_dispatch;
  CacheAwareDispatchObservation cache_aware_dispatch;
  CacheAwareDispatchObservation cache_aware_stale_dispatch;
  CacheAwareDispatchObservation cache_aware_malformed_dispatch;
  CacheAwareDispatchObservation cache_aware_missing_validation_dispatch;
  MethodCacheEntryObservation dynamic_entry;
  MethodCacheEntryObservation explicit_entry;
  MethodCacheEntryObservation strict_error_entry;
  int implicit_value = 0;
  int explicit_value = 0;
  int mixed_first_value = 0;
  int mixed_second_value = 0;
  int strict_error_expected = 0;
  int strict_error_first_value = 0;
  int strict_error_second_value = 0;
  int cache_aware_value = 0;
  int cache_aware_stale_value = 0;
  int cache_aware_malformed_status = 0;
  int cache_aware_missing_validation_status = 0;
};

inline std::string CopyRuntimeString(const char *value) {
  return value != nullptr ? value : "";
}

} // namespace live_dispatch_fast_path_probe
} // namespace tooling
} // namespace objc3c
