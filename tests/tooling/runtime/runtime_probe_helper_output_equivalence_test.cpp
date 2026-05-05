#include "support/json_probe_writer.h"
#include "support/output_expectations.h"
#include "support/runtime_snapshot_text.h"

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <sstream>

int main() {
  using objc3c::runtime::probe::ExpectTextEqual;
  using objc3c::runtime::probe::JsonFieldSeparator;
  using objc3c::runtime::probe::WriteJsonBoolField;
  using objc3c::runtime::probe::WriteJsonIntField;
  using objc3c::runtime::probe::WriteJsonStringField;
  using objc3c::runtime::probe::WriteLabeledDispatchState;
  using objc3c::runtime::probe::WriteLabeledFastPathMethodCacheState;
  using objc3c::runtime::probe::WriteLabeledMethodCacheState;

  std::ostringstream json;
  JsonFieldSeparator fields;
  json << '{';
  WriteJsonStringField(json, fields, "selector", "copy");
  WriteJsonIntField(json, fields, "strict_error", 22535);
  WriteJsonBoolField(json, fields, "cache_hit", true);
  WriteJsonStringField(json, fields, "missing", nullptr);
  json << '}';
  if (ExpectTextEqual(
          json.str(),
          "{\"selector\":\"copy\",\"strict_error\":22535,\"cache_hit\":true,"
          "\"missing\":null}",
          "representative JSON helper output", 1) != 0) {
    return 1;
  }

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

  std::ostringstream method_cache;
  WriteLabeledMethodCacheState(method_cache, "direct", cache, "copy");
  if (ExpectTextEqual(method_cache.str(),
                      "direct_cache_entry_count=3\n"
                      "direct_cache_hit_count=5\n"
                      "direct_cache_miss_count=2\n"
                      "direct_slow_path_lookup_count=2\n"
                      "direct_live_dispatch_count=7\n"
                      "direct_strict_dispatch_error_count=1\n"
                      "direct_last_dispatch_used_cache=1\n"
                      "direct_last_dispatch_resolved_live_method=1\n"
                      "direct_last_dispatch_strict_error=0\n"
                      "direct_last_selector=copy\n",
                      "labeled method cache state output", 2) != 0) {
    return 2;
  }

  cache.fast_path_seed_count = 4;
  cache.fast_path_hit_count = 6;
  cache.last_dispatch_used_fast_path = 1;
  std::ostringstream fast_path_cache;
  WriteLabeledFastPathMethodCacheState(fast_path_cache, "fast", cache, "copy",
                                       "selector-table-hit");
  if (ExpectTextEqual(fast_path_cache.str(),
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
                      "fast_last_fast_path_reason=selector-table-hit\n",
                      "labeled fast-path cache state output", 3) != 0) {
    return 3;
  }

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
  std::ostringstream dispatch_state;
  WriteLabeledDispatchState(dispatch_state, "mixed", dispatch, "copy",
                            "selector-table-hit", "cache", "method",
                            "Widget");
  if (ExpectTextEqual(dispatch_state.str(),
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
                      "mixed_last_resolved_class_name=Widget\n",
                      "labeled dispatch state output", 4) != 0) {
    return 4;
  }

  return 0;
}
