#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_PROBE_HELPER_OUTPUT_EQUIVALENCE_TEST_RUNTIME_SNAPSHOT_FIXTURES_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_PROBE_HELPER_OUTPUT_EQUIVALENCE_TEST_RUNTIME_SNAPSHOT_FIXTURES_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime::probe::runtime_probe_helper_output_equivalence {

inline objc3_runtime_method_cache_state_snapshot
MakeRepresentativeMethodCacheState() {
  objc3_runtime_method_cache_state_snapshot cache{};
  cache.cache_entry_count = 3;
  cache.cache_hit_count = 5;
  cache.cache_miss_count = 2;
  cache.slow_path_lookup_count = 2;
  cache.live_dispatch_count = 7;
  cache.strict_dispatch_error_count = 1;
  cache.last_dispatch_used_cache = 1;
  cache.last_dispatch_resolved_live_method = 1;
  cache.last_dispatch_strict_error = 0;
  return cache;
}

inline objc3_runtime_method_cache_state_snapshot
MakeRepresentativeFastPathMethodCacheState() {
  objc3_runtime_method_cache_state_snapshot cache =
      MakeRepresentativeMethodCacheState();
  cache.fast_path_seed_count = 4;
  cache.fast_path_hit_count = 6;
  cache.last_dispatch_used_fast_path = 1;
  return cache;
}

inline objc3_runtime_dispatch_state_snapshot
MakeRepresentativeDispatchState() {
  objc3_runtime_dispatch_state_snapshot dispatch{};
  dispatch.cache_entry_count = 8;
  dispatch.fast_path_seed_count = 9;
  dispatch.fast_path_hit_count = 10;
  dispatch.live_dispatch_count = 11;
  dispatch.strict_dispatch_error_count = 12;
  dispatch.last_resolved_parameter_count = 4;
  dispatch.last_dispatch_used_cache = 1;
  dispatch.last_dispatch_used_fast_path = 1;
  dispatch.last_dispatch_resolved_live_method = 1;
  dispatch.last_dispatch_strict_error = 0;
  dispatch.last_effective_direct_dispatch = 1;
  dispatch.last_used_builtin = 0;
  return dispatch;
}

}  // namespace objc3c::runtime::probe::runtime_probe_helper_output_equivalence

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_PROBE_HELPER_OUTPUT_EQUIVALENCE_TEST_RUNTIME_SNAPSHOT_FIXTURES_H_
