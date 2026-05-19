#pragma once

#include "report_helpers.h"
#include "slow_path_cache_setup.h"

#include <cstdio>
#include <cstdint>
#include <cstring>

namespace objc3c {
namespace tooling {
namespace method_cache_slow_path_probe {

inline bool CacheStateAdvanced(
    const objc3_runtime_method_cache_state_snapshot &after,
    const objc3_runtime_method_cache_state_snapshot &before,
    std::uint64_t objc3_runtime_method_cache_state_snapshot::*field) {
  return after.*field > before.*field;
}

inline bool LastSelectorMatches(
    const objc3_runtime_method_cache_state_snapshot &state,
    const char *expected) {
  return state.last_selector != nullptr &&
         std::strcmp(state.last_selector, expected) == 0;
}

inline bool ValidateMethodCacheSlowPathProbe(const SlowPathProbeRun &run) {
  if (run.registration.state.registered_image_count == 0 ||
      run.instance_first != 11 || run.instance_second != 11 ||
      run.instance_after_stale != 11 || run.class_self != 22 ||
      run.known_class != 22 ||
      run.strict_error_first != run.strict_error_expected ||
      run.strict_error_second != run.strict_error_expected) {
    std::fprintf(stderr, "method-cache probe value invariant failed\n");
    return false;
  }
  const auto &first = run.instance_first_state.state;
  const auto &second = run.instance_second_state.state;
  const auto &after_stale = run.instance_after_stale_state.state;
  if (first.last_dispatch_used_cache != 0 ||
      first.last_dispatch_resolved_live_method != 1 ||
      first.last_dispatch_strict_error != 0 ||
      !LastSelectorMatches(first, kCurrentValueSelector)) {
    std::fprintf(stderr, "method-cache first dispatch invariant failed\n");
    return false;
  }
  if (!CacheStateAdvanced(second, first,
                          &objc3_runtime_method_cache_state_snapshot::
                              cache_hit_count) ||
      second.last_dispatch_used_cache != 1 ||
      second.last_dispatch_resolved_live_method != 1 ||
      second.last_dispatch_strict_error != 0 ||
      !LastSelectorMatches(second, kCurrentValueSelector)) {
    std::fprintf(stderr, "method-cache cache-hit invariant failed\n");
    return false;
  }
  if (!CacheStateAdvanced(
          after_stale, second,
          &objc3_runtime_method_cache_state_snapshot::
              stale_method_cache_entry_count) ||
      !CacheStateAdvanced(after_stale, second,
                          &objc3_runtime_method_cache_state_snapshot::
                              cache_hit_count) ||
      !CacheStateAdvanced(after_stale, second,
                          &objc3_runtime_method_cache_state_snapshot::
                              cache_miss_count) ||
      !CacheStateAdvanced(after_stale, second,
                          &objc3_runtime_method_cache_state_snapshot::
                              slow_path_lookup_count) ||
      after_stale.last_dispatch_resolved_live_method != 1 ||
      after_stale.last_dispatch_strict_error != 0 ||
      !LastSelectorMatches(after_stale, kCurrentValueSelector)) {
    std::fprintf(stderr, "method-cache stale revalidation invariant failed\n");
    return false;
  }
  const auto &strict_first = run.strict_error_first_state.state;
  const auto &strict_second = run.strict_error_second_state.state;
  if (!CacheStateAdvanced(strict_second, strict_first,
                          &objc3_runtime_method_cache_state_snapshot::
                              cache_hit_count) ||
      strict_second.last_dispatch_used_cache != 1 ||
      strict_second.last_dispatch_resolved_live_method != 0 ||
      strict_second.last_dispatch_strict_error != 1 ||
      !LastSelectorMatches(strict_second, kStrictErrorSelector)) {
    std::fprintf(stderr, "method-cache strict-error cache invariant failed\n");
    return false;
  }
  return run.instance_entry.entry.found == 1 &&
         run.instance_after_stale_entry.entry.found == 1 &&
         run.class_entry.entry.found == 1 &&
         run.strict_error_entry.entry.found == 1;
}

inline int RunMethodCacheSlowPathProbe() {
  SlowPathProbeRun run{};
  CaptureSlowPathProbeRun(run);
  PrintMethodCacheSlowPathProbeReport(run);
  return ValidateMethodCacheSlowPathProbe(run) ? 0 : 1;
}

} // namespace method_cache_slow_path_probe
} // namespace tooling
} // namespace objc3c
