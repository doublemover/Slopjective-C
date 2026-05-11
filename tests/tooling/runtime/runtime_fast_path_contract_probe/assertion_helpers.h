#pragma once

#include "fixture_types.h"

namespace objc3c {
namespace tooling {
namespace runtime_fast_path_contract_probe {

inline bool RuntimeSnapshotCopiesSucceeded(const ProbeRun &run) {
  return run.baseline.status == 0 && run.direct.status == 0 &&
         run.mixed_first.status == 0 && run.mixed_second.status == 0 &&
         run.strict_error_first.status == 0 &&
         run.strict_error_second.status == 0 &&
         run.dynamic_entry.status == 0 && run.strict_error_entry.status == 0;
}

inline bool FixtureReturnValuesMatch(const ProbeRun &run) {
  return run.implicit_value == 3 && run.explicit_value == 5 &&
         run.mixed_first_value == 12 && run.mixed_second_value == 12 &&
         run.strict_error_first_value == run.strict_error_expected &&
         run.strict_error_second_value == run.strict_error_expected;
}

inline bool DirectDispatchLeavesRuntimeCountersUnchanged(const ProbeRun &run) {
  const auto &baseline = run.baseline.state;
  const auto &direct = run.direct.state;

  return direct.cache_entry_count == baseline.cache_entry_count &&
         direct.cache_hit_count == baseline.cache_hit_count &&
         direct.cache_miss_count == baseline.cache_miss_count &&
         direct.slow_path_lookup_count == baseline.slow_path_lookup_count &&
         direct.live_dispatch_count == baseline.live_dispatch_count &&
         direct.strict_dispatch_error_count ==
             baseline.strict_dispatch_error_count;
}

inline bool FirstMixedDispatchHitsCachedRuntimeMethod(const ProbeRun &run) {
  const auto &direct = run.direct.state;
  const auto &mixed = run.mixed_first.state;

  return mixed.cache_entry_count == direct.cache_entry_count &&
         mixed.cache_hit_count == direct.cache_hit_count + 1 &&
         mixed.cache_miss_count == direct.cache_miss_count &&
         mixed.slow_path_lookup_count == direct.slow_path_lookup_count &&
         mixed.live_dispatch_count == direct.live_dispatch_count + 1 &&
         mixed.strict_dispatch_error_count ==
             direct.strict_dispatch_error_count &&
         mixed.last_dispatch_used_cache == 1 &&
         mixed.last_dispatch_resolved_live_method == 1 &&
         mixed.last_dispatch_strict_error == 0;
}

inline bool SecondMixedDispatchHitsCachedRuntimeMethod(const ProbeRun &run) {
  const auto &first = run.mixed_first.state;
  const auto &second = run.mixed_second.state;

  return second.cache_entry_count == first.cache_entry_count &&
         second.cache_hit_count == first.cache_hit_count + 1 &&
         second.cache_miss_count == first.cache_miss_count &&
         second.live_dispatch_count == first.live_dispatch_count + 1 &&
         second.last_dispatch_used_cache == 1 &&
         second.last_dispatch_resolved_live_method == 1 &&
         second.last_dispatch_strict_error == 0;
}

inline bool FirstStrictDispatchErrorSeedsCacheEntry(const ProbeRun &run) {
  const auto &mixed = run.mixed_second.state;
  const auto &strict = run.strict_error_first.state;

  return strict.cache_entry_count == mixed.cache_entry_count + 1 &&
         strict.cache_miss_count == mixed.cache_miss_count + 1 &&
         strict.slow_path_lookup_count == mixed.slow_path_lookup_count + 1 &&
         strict.strict_dispatch_error_count ==
             mixed.strict_dispatch_error_count + 1 &&
         strict.last_dispatch_used_cache == 0 &&
         strict.last_dispatch_resolved_live_method == 0 &&
         strict.last_dispatch_strict_error == 1;
}

inline bool SecondStrictDispatchErrorHitsCachedErrorEntry(const ProbeRun &run) {
  const auto &first = run.strict_error_first.state;
  const auto &second = run.strict_error_second.state;

  return second.cache_entry_count == first.cache_entry_count &&
         second.cache_hit_count == first.cache_hit_count + 1 &&
         second.cache_miss_count == first.cache_miss_count &&
         second.strict_dispatch_error_count ==
             first.strict_dispatch_error_count + 1 &&
         second.last_dispatch_used_cache == 1 &&
         second.last_dispatch_resolved_live_method == 0 &&
         second.last_dispatch_strict_error == 1;
}

inline bool CacheEntriesMatch(const ProbeRun &run) {
  const auto &dynamic_entry = run.dynamic_entry.entry;
  const auto &strict_error_entry = run.strict_error_entry.entry;

  return dynamic_entry.found == 1 && dynamic_entry.resolved == 1 &&
         strict_error_entry.found == 1 && strict_error_entry.resolved == 0;
}

inline bool ProbeAssertionsPassed(const ProbeRun &run) {
  return RuntimeSnapshotCopiesSucceeded(run) && FixtureReturnValuesMatch(run) &&
         DirectDispatchLeavesRuntimeCountersUnchanged(run) &&
         FirstMixedDispatchHitsCachedRuntimeMethod(run) &&
         SecondMixedDispatchHitsCachedRuntimeMethod(run) &&
         FirstStrictDispatchErrorSeedsCacheEntry(run) &&
         SecondStrictDispatchErrorHitsCachedErrorEntry(run) &&
         CacheEntriesMatch(run);
}

} // namespace runtime_fast_path_contract_probe
} // namespace tooling
} // namespace objc3c
