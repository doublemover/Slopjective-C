#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_PROBE_HELPER_OUTPUT_EQUIVALENCE_TEST_EXPECTED_OUTPUT_FIXTURES_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_PROBE_HELPER_OUTPUT_EQUIVALENCE_TEST_EXPECTED_OUTPUT_FIXTURES_H_

namespace objc3c::runtime::probe::runtime_probe_helper_output_equivalence {

inline constexpr char kRepresentativeJsonOutput[] =
    "{\"selector\":\"copy\",\"strict_error\":22535,\"cache_hit\":true,"
    "\"missing\":null}";

inline constexpr char kLabeledMethodCacheStateOutput[] =
    "direct_cache_entry_count=3\n"
    "direct_cache_hit_count=5\n"
    "direct_cache_miss_count=2\n"
    "direct_slow_path_lookup_count=2\n"
    "direct_live_dispatch_count=7\n"
    "direct_strict_dispatch_error_count=1\n"
    "direct_last_dispatch_used_cache=1\n"
    "direct_last_dispatch_resolved_live_method=1\n"
    "direct_last_dispatch_strict_error=0\n"
    "direct_last_selector=copy\n";

inline constexpr char kLabeledFastPathMethodCacheStateOutput[] =
    "fast_cache_entry_count=3\n"
    "fast_cache_hit_count=5\n"
    "fast_cache_miss_count=2\n"
    "fast_slow_path_lookup_count=2\n"
    "fast_live_dispatch_count=7\n"
    "fast_strict_dispatch_error_count=1\n"
    "fast_fast_path_seed_count=4\n"
    "fast_fast_path_hit_count=6\n"
    "fast_last_dispatch_used_cache=1\n"
    "fast_last_dispatch_used_fast_path=1\n"
    "fast_last_dispatch_resolved_live_method=1\n"
    "fast_last_dispatch_strict_error=0\n"
    "fast_last_selector=copy\n"
    "fast_last_fast_path_reason=selector-table-hit\n";

inline constexpr char kLabeledDispatchStateOutput[] =
    "mixed_cache_entry_count=8\n"
    "mixed_fast_path_seed_count=9\n"
    "mixed_fast_path_hit_count=10\n"
    "mixed_live_dispatch_count=11\n"
    "mixed_strict_dispatch_error_count=12\n"
    "mixed_last_resolved_parameter_count=4\n"
    "mixed_last_dispatch_used_cache=1\n"
    "mixed_last_dispatch_used_fast_path=1\n"
    "mixed_last_dispatch_resolved_live_method=1\n"
    "mixed_last_dispatch_strict_error=0\n"
    "mixed_last_effective_direct_dispatch=1\n"
    "mixed_last_used_builtin=0\n"
    "mixed_last_selector=copy\n"
    "mixed_last_fast_path_reason=selector-table-hit\n"
    "mixed_last_dispatch_path=cache\n"
    "mixed_last_implementation_kind=method\n"
    "mixed_last_resolved_class_name=Widget\n";

}  // namespace objc3c::runtime::probe::runtime_probe_helper_output_equivalence

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_PROBE_HELPER_OUTPUT_EQUIVALENCE_TEST_EXPECTED_OUTPUT_FIXTURES_H_
