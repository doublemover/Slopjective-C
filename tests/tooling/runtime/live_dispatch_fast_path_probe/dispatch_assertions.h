#pragma once

#include "fixture_types.h"

namespace objc3c {
namespace tooling {
namespace live_dispatch_fast_path_probe {

inline bool RuntimeSnapshotCopiesSucceeded(const ProbeRun &run) {
  return run.baseline.status == 0 && run.dynamic_entry.status == 0 &&
         run.explicit_entry.status == 0 && run.direct.status == 0 &&
         run.mixed_first.status == 0 && run.mixed_second.status == 0 &&
         run.mixed_first_dispatch.status == 0 &&
         run.mixed_second_dispatch.status == 0 &&
         run.strict_error_first.status == 0 &&
         run.strict_error_second.status == 0 &&
         run.strict_error_first_dispatch.status == 0 &&
         run.strict_error_second_dispatch.status == 0 &&
         run.strict_error_entry.status == 0 &&
         run.cache_aware_dispatch.status == 0 &&
         run.cache_aware_stale_dispatch.status == 0 &&
         run.cache_aware_malformed_dispatch.status == 0;
}

inline bool FixtureReturnValuesMatch(const ProbeRun &run) {
  return run.implicit_value == 3 && run.explicit_value == 5 &&
         run.mixed_first_value == 12 && run.mixed_second_value == 12 &&
         run.strict_error_first_value == run.strict_error_expected &&
         run.strict_error_second_value == run.strict_error_expected &&
         run.cache_aware_value == 12 &&
         run.cache_aware_stale_value == 12 &&
         run.cache_aware_malformed_status ==
             OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA;
}

inline bool SeededEntriesMatch(const ProbeRun &run) {
  const auto &baseline = run.baseline.state;
  const auto &dynamic_entry = run.dynamic_entry.entry;
  const auto &explicit_entry = run.explicit_entry.entry;

  return baseline.cache_entry_count == 4 && baseline.fast_path_seed_count == 4 &&
         dynamic_entry.found == 1 && dynamic_entry.resolved == 1 &&
         dynamic_entry.fast_path_seeded == 1 &&
         dynamic_entry.effective_direct_dispatch == 0 &&
         dynamic_entry.objc_final_declared == 1 &&
         dynamic_entry.objc_sealed_declared == 1 &&
         run.dynamic_entry.fast_path_reason == "class-final" &&
         explicit_entry.found == 1 && explicit_entry.resolved == 1 &&
         explicit_entry.fast_path_seeded == 1 &&
         explicit_entry.effective_direct_dispatch == 1 &&
         run.explicit_entry.fast_path_reason == "direct";
}

inline bool RuntimeCacheAbiFieldsMatch(const ProbeRun &run) {
  const auto &baseline = run.baseline.state;
  const auto &direct = run.direct.state;
  const auto &strict_first = run.strict_error_first.state;
  const auto &dynamic_entry = run.dynamic_entry.entry;
  const auto &explicit_entry = run.explicit_entry.entry;
  const auto &strict_entry = run.strict_error_entry.entry;

  const bool baseline_invalidation_is_initial_or_reset =
      baseline.last_invalidation_reason ==
          OBJC3_RUNTIME_METHOD_CACHE_INVALIDATION_NONE ||
      baseline.last_invalidation_reason ==
          OBJC3_RUNTIME_METHOD_CACHE_INVALIDATION_RESET;
  const bool strict_invalidation_is_initial_or_reset =
      strict_first.last_invalidation_reason ==
          baseline.last_invalidation_reason;

  return baseline.abi_version == OBJC3_RUNTIME_METHOD_CACHE_ABI_VERSION &&
         direct.abi_version == OBJC3_RUNTIME_METHOD_CACHE_ABI_VERSION &&
         strict_first.abi_version == OBJC3_RUNTIME_METHOD_CACHE_ABI_VERSION &&
         dynamic_entry.abi_version == OBJC3_RUNTIME_METHOD_CACHE_ABI_VERSION &&
         explicit_entry.abi_version == OBJC3_RUNTIME_METHOD_CACHE_ABI_VERSION &&
         strict_entry.abi_version == OBJC3_RUNTIME_METHOD_CACHE_ABI_VERSION &&
         baseline.next_cache_entry_generation ==
             baseline.cache_entry_count + 1 &&
         direct.next_cache_entry_generation ==
             baseline.next_cache_entry_generation &&
         strict_first.next_cache_entry_generation ==
             strict_first.cache_entry_count + 1 &&
         baseline_invalidation_is_initial_or_reset &&
         strict_invalidation_is_initial_or_reset &&
         dynamic_entry.cache_entry_generation != 0 &&
         explicit_entry.cache_entry_generation != 0 &&
         strict_entry.cache_entry_generation != 0 &&
         dynamic_entry.miss_status == OBJC3_RUNTIME_DISPATCH_STATUS_OK &&
         explicit_entry.miss_status == OBJC3_RUNTIME_DISPATCH_STATUS_OK &&
         strict_entry.miss_status ==
             OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR;
}

inline bool DirectCallsLeaveRuntimeCountersUnchanged(const ProbeRun &run) {
  const auto &baseline = run.baseline.state;
  const auto &direct = run.direct.state;

  return direct.cache_entry_count == baseline.cache_entry_count &&
         direct.cache_hit_count == baseline.cache_hit_count &&
         direct.cache_miss_count == baseline.cache_miss_count &&
         direct.slow_path_lookup_count == baseline.slow_path_lookup_count &&
         direct.fast_path_hit_count == baseline.fast_path_hit_count &&
         direct.live_dispatch_count == baseline.live_dispatch_count;
}

inline bool FirstMixedDispatchHitsClassFinalFastPath(const ProbeRun &run) {
  const auto &direct = run.direct.state;
  const auto &mixed = run.mixed_first.state;
  const auto &dispatch = run.mixed_first_dispatch.state;

  return mixed.cache_entry_count == direct.cache_entry_count &&
         mixed.cache_hit_count == direct.cache_hit_count + 1 &&
         mixed.cache_miss_count == direct.cache_miss_count &&
         mixed.slow_path_lookup_count == direct.slow_path_lookup_count &&
         mixed.fast_path_hit_count == direct.fast_path_hit_count + 1 &&
         mixed.live_dispatch_count == direct.live_dispatch_count + 1 &&
         mixed.last_dispatch_used_cache == 1 &&
         mixed.last_dispatch_used_fast_path == 1 &&
         mixed.last_dispatch_resolved_live_method == 1 &&
         mixed.last_dispatch_strict_error == 0 &&
         run.mixed_first.last_selector == "dynamicEscape" &&
         run.mixed_first.last_fast_path_reason == "class-final" &&
         run.mixed_first_dispatch.last_path == "cache-hit-fast-path" &&
         run.mixed_first_dispatch.last_implementation_kind ==
             "emitted-method-body" &&
         dispatch.last_effective_direct_dispatch == 0 &&
         dispatch.last_used_builtin == 0 &&
         dispatch.last_resolved_parameter_count == 0 &&
         run.mixed_first_dispatch.last_resolved_class_name == "PolicyBox";
}

inline bool SecondMixedDispatchReusesClassFinalFastPath(const ProbeRun &run) {
  const auto &first = run.mixed_first.state;
  const auto &second = run.mixed_second.state;
  const auto &dispatch = run.mixed_second_dispatch.state;

  return second.cache_entry_count == first.cache_entry_count &&
         second.cache_hit_count == first.cache_hit_count + 1 &&
         second.fast_path_hit_count == first.fast_path_hit_count + 1 &&
         second.live_dispatch_count == first.live_dispatch_count + 1 &&
         second.last_dispatch_used_cache == 1 &&
         second.last_dispatch_used_fast_path == 1 &&
         run.mixed_second.last_selector == "dynamicEscape" &&
         run.mixed_second.last_fast_path_reason == "class-final" &&
         run.mixed_second_dispatch.last_path == "cache-hit-fast-path" &&
         run.mixed_second_dispatch.last_implementation_kind ==
             "emitted-method-body" &&
         dispatch.last_effective_direct_dispatch == 0 &&
         dispatch.last_used_builtin == 0 &&
         dispatch.last_resolved_parameter_count == 0 &&
         run.mixed_second_dispatch.last_resolved_class_name == "PolicyBox";
}

inline bool FirstStrictDispatchErrorSeedsStrictErrorEntry(const ProbeRun &run) {
  const auto &mixed = run.mixed_second.state;
  const auto &strict = run.strict_error_first.state;
  const auto &dispatch = run.strict_error_first_dispatch.state;

  return strict.cache_entry_count == mixed.cache_entry_count + 1 &&
         strict.cache_miss_count == mixed.cache_miss_count + 1 &&
         strict.slow_path_lookup_count == mixed.slow_path_lookup_count + 1 &&
         strict.strict_dispatch_error_count ==
             mixed.strict_dispatch_error_count + 1 &&
         strict.last_dispatch_used_cache == 0 &&
         strict.last_dispatch_used_fast_path == 0 &&
         strict.last_dispatch_resolved_live_method == 0 &&
         strict.last_dispatch_strict_error == 1 &&
         run.strict_error_first.last_selector == "missingDispatch:" &&
         run.strict_error_first.last_fast_path_reason.empty() &&
         run.strict_error_first_dispatch.last_path == "slow-path-error" &&
         run.strict_error_first_dispatch.last_implementation_kind ==
             "strict-dispatch-error" &&
         dispatch.last_effective_direct_dispatch == 0 &&
         dispatch.last_used_builtin == 0 &&
         dispatch.last_resolved_parameter_count == 0 &&
         run.strict_error_first_dispatch.last_resolved_class_name.empty();
}

inline bool SecondStrictDispatchErrorHitsCachedStrictErrorEntry(
    const ProbeRun &run) {
  const auto &first = run.strict_error_first.state;
  const auto &second = run.strict_error_second.state;
  const auto &dispatch = run.strict_error_second_dispatch.state;

  return second.cache_entry_count == first.cache_entry_count &&
         second.cache_hit_count == first.cache_hit_count + 1 &&
         second.strict_dispatch_error_count ==
             first.strict_dispatch_error_count + 1 &&
         second.last_dispatch_used_cache == 1 &&
         second.last_dispatch_used_fast_path == 0 &&
         second.last_dispatch_resolved_live_method == 0 &&
         second.last_dispatch_strict_error == 1 &&
         run.strict_error_second.last_selector == "missingDispatch:" &&
         run.strict_error_second.last_fast_path_reason.empty() &&
         run.strict_error_second_dispatch.last_path == "cache-hit-error" &&
         run.strict_error_second_dispatch.last_implementation_kind ==
             "strict-dispatch-error" &&
         dispatch.last_effective_direct_dispatch == 0 &&
         dispatch.last_used_builtin == 0 &&
         dispatch.last_resolved_parameter_count == 0 &&
         run.strict_error_second_dispatch.last_resolved_class_name.empty();
}

inline bool StrictErrorCacheEntryMatches(const ProbeRun &run) {
  const auto &entry = run.strict_error_entry.entry;

  return entry.found == 1 && entry.resolved == 0 &&
         entry.fast_path_seeded == 0 &&
         run.strict_error_entry.fast_path_reason.empty();
}

inline bool CacheAwareDispatchRecordsMatch(const ProbeRun &run) {
  const auto &valid = run.cache_aware_dispatch.record;
  const auto &stale = run.cache_aware_stale_dispatch.record;
  const auto &malformed = run.cache_aware_malformed_dispatch.record;
  const char *expected_source =
      "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3";

  return valid.abi_version == OBJC3_RUNTIME_CACHE_AWARE_DISPATCH_ABI_VERSION &&
         valid.descriptor_valid == 1 &&
         valid.fallback_used == 0 &&
         valid.used_cache == 1 &&
         valid.used_fast_path == 1 &&
         valid.strict_error == 0 &&
         valid.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK &&
         valid.invalidation_reason ==
             OBJC3_RUNTIME_METHOD_CACHE_INVALIDATION_NONE &&
         valid.selector_stable_id != 0 &&
         valid.normalized_receiver_identity != 0 &&
         valid.cache_entry_generation ==
             run.dynamic_entry.entry.cache_entry_generation &&
         valid.class_graph_generation ==
             run.mixed_second.state.class_graph_generation &&
         valid.category_attachment_generation ==
             run.mixed_second.state.category_attachment_generation &&
         valid.protocol_declaration_generation ==
             run.mixed_second.state.protocol_declaration_generation &&
         valid.storage_surface_generation ==
             run.mixed_second.state.storage_surface_generation &&
         valid.method_surface_generation ==
             run.mixed_second.state.method_surface_generation &&
         valid.method_target_identity != 0 &&
         valid.source_line == 1 &&
         valid.source_column == 1 &&
         run.cache_aware_dispatch.selector == "dynamicEscape" &&
         run.cache_aware_dispatch.source_path == expected_source &&
         run.cache_aware_dispatch.dispatch_path == "cache-hit-fast-path" &&
         run.cache_aware_dispatch.implementation_kind ==
             "emitted-method-body" &&
         run.cache_aware_dispatch.diagnostic_code ==
             "objc3.runtime.dispatch.ok" &&
         stale.abi_version == OBJC3_RUNTIME_CACHE_AWARE_DISPATCH_ABI_VERSION &&
         stale.descriptor_valid == 0 &&
         stale.fallback_used == 1 &&
         stale.used_cache == 1 &&
         stale.used_fast_path == 1 &&
         stale.strict_error == 0 &&
         stale.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK &&
         stale.invalidation_reason ==
             OBJC3_RUNTIME_METHOD_CACHE_INVALIDATION_STALE_GENERATION &&
         stale.cache_entry_generation ==
             run.dynamic_entry.entry.cache_entry_generation &&
         stale.method_target_identity == valid.method_target_identity &&
         run.cache_aware_stale_dispatch.selector == "dynamicEscape" &&
         run.cache_aware_stale_dispatch.source_path == expected_source &&
         run.cache_aware_stale_dispatch.dispatch_path == "cache-hit-fast-path" &&
         malformed.abi_version ==
             OBJC3_RUNTIME_CACHE_AWARE_DISPATCH_ABI_VERSION &&
         malformed.descriptor_valid == 0 &&
         malformed.fallback_used == 0 &&
         malformed.used_cache == 0 &&
         malformed.used_fast_path == 0 &&
         malformed.strict_error == 0 &&
         malformed.status_code ==
             OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA &&
         malformed.invalidation_reason ==
             OBJC3_RUNTIME_METHOD_CACHE_INVALIDATION_NONE &&
         malformed.cache_entry_generation == 0 &&
         malformed.method_target_identity == 0 &&
         run.cache_aware_malformed_dispatch.selector == "dynamicEscape" &&
         run.cache_aware_malformed_dispatch.source_path == expected_source &&
         run.cache_aware_malformed_dispatch.dispatch_path ==
             "cache-aware-descriptor-error" &&
         run.cache_aware_malformed_dispatch.implementation_kind ==
             "strict-dispatch-error" &&
         run.cache_aware_malformed_dispatch.diagnostic_code ==
             "objc3.runtime.dispatch.malformed_metadata";
}

inline bool ProbeAssertionsPassed(const ProbeRun &run) {
  return RuntimeSnapshotCopiesSucceeded(run) && FixtureReturnValuesMatch(run) &&
         SeededEntriesMatch(run) &&
         RuntimeCacheAbiFieldsMatch(run) &&
         DirectCallsLeaveRuntimeCountersUnchanged(run) &&
         FirstMixedDispatchHitsClassFinalFastPath(run) &&
         SecondMixedDispatchReusesClassFinalFastPath(run) &&
         FirstStrictDispatchErrorSeedsStrictErrorEntry(run) &&
         SecondStrictDispatchErrorHitsCachedStrictErrorEntry(run) &&
         StrictErrorCacheEntryMatches(run) &&
         CacheAwareDispatchRecordsMatch(run);
}

} // namespace live_dispatch_fast_path_probe
} // namespace tooling
} // namespace objc3c
