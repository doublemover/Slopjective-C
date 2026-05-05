#include <iostream>
#include <string>

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/dispatch_expectations.h"
#include "support/runtime_snapshot_text.h"

extern "C" int callImplicit(void);
extern "C" int callExplicit(void);
extern "C" int callMixed(void);

namespace {

using objc3c::runtime::probe::WriteLabeledMethodCacheState;

using objc3c::runtime::probe::ExpectedStrictDispatchErrorValue;

} // namespace

int main() {
  objc3_runtime_method_cache_state_snapshot baseline{};
  objc3_runtime_method_cache_state_snapshot direct_state{};
  objc3_runtime_method_cache_state_snapshot mixed_first_state{};
  objc3_runtime_method_cache_state_snapshot mixed_second_state{};
  objc3_runtime_method_cache_state_snapshot fallback_first_state{};
  objc3_runtime_method_cache_state_snapshot fallback_second_state{};
  objc3_runtime_method_cache_entry_snapshot dynamic_entry{};
  objc3_runtime_method_cache_entry_snapshot fallback_entry{};
  std::string baseline_last_selector;
  std::string direct_last_selector;
  std::string mixed_first_last_selector;
  std::string mixed_second_last_selector;
  std::string fallback_first_last_selector;
  std::string fallback_second_last_selector;

  const int baseline_status =
      objc3_runtime_copy_method_cache_state_for_testing(&baseline);
  baseline_last_selector =
      baseline.last_selector != nullptr ? baseline.last_selector : "";
  std::cout << "stage=baseline" << std::endl;
  const int implicit_value = callImplicit();
  const int explicit_value = callExplicit();
  std::cout << "stage=direct" << std::endl;
  const int direct_status =
      objc3_runtime_copy_method_cache_state_for_testing(&direct_state);
  direct_last_selector =
      direct_state.last_selector != nullptr ? direct_state.last_selector : "";
  const int mixed_first = callMixed();
  std::cout << "stage=mixed_first_call" << std::endl;
  const int mixed_first_status =
      objc3_runtime_copy_method_cache_state_for_testing(&mixed_first_state);
  mixed_first_last_selector = mixed_first_state.last_selector != nullptr
                                  ? mixed_first_state.last_selector
                                  : "";
  const int mixed_second = callMixed();
  std::cout << "stage=mixed_second_call" << std::endl;
  const int mixed_second_status =
      objc3_runtime_copy_method_cache_state_for_testing(&mixed_second_state);
  mixed_second_last_selector = mixed_second_state.last_selector != nullptr
                                   ? mixed_second_state.last_selector
                                   : "";

  const char *const fallback_selector = "missingDispatch:";
  const int fallback_expected =
      ExpectedStrictDispatchErrorValue(1024, fallback_selector, 4, 5, 6, 7);
  const int fallback_first =
      objc3_runtime_dispatch_i32(1024, fallback_selector, 4, 5, 6, 7);
  std::cout << "stage=fallback_first_call" << std::endl;
  const int fallback_first_status =
      objc3_runtime_copy_method_cache_state_for_testing(&fallback_first_state);
  fallback_first_last_selector = fallback_first_state.last_selector != nullptr
                                     ? fallback_first_state.last_selector
                                     : "";
  const int fallback_second =
      objc3_runtime_dispatch_i32(1024, fallback_selector, 4, 5, 6, 7);
  std::cout << "stage=fallback_second_call" << std::endl;
  const int fallback_second_status =
      objc3_runtime_copy_method_cache_state_for_testing(&fallback_second_state);
  fallback_second_last_selector = fallback_second_state.last_selector != nullptr
                                      ? fallback_second_state.last_selector
                                      : "";

  const int dynamic_entry_status =
      objc3_runtime_copy_method_cache_entry_for_testing(1024, "dynamicEscape",
                                                        &dynamic_entry);
  const int fallback_entry_status =
      objc3_runtime_copy_method_cache_entry_for_testing(1024, fallback_selector,
                                                        &fallback_entry);

  std::cout << "baseline_status=" << baseline_status << "\n";
  std::cout << "implicit_value=" << implicit_value << "\n";
  std::cout << "explicit_value=" << explicit_value << "\n";
  std::cout << "direct_status=" << direct_status << "\n";
  std::cout << "mixed_first=" << mixed_first << "\n";
  std::cout << "mixed_first_status=" << mixed_first_status << "\n";
  std::cout << "mixed_second=" << mixed_second << "\n";
  std::cout << "mixed_second_status=" << mixed_second_status << "\n";
  std::cout << "fallback_expected=" << fallback_expected << "\n";
  std::cout << "fallback_first=" << fallback_first << "\n";
  std::cout << "fallback_first_status=" << fallback_first_status << "\n";
  std::cout << "fallback_second=" << fallback_second << "\n";
  std::cout << "fallback_second_status=" << fallback_second_status << "\n";
  std::cout << "dynamic_entry_status=" << dynamic_entry_status << "\n";
  std::cout << "dynamic_entry_found=" << dynamic_entry.found << "\n";
  std::cout << "dynamic_entry_resolved=" << dynamic_entry.resolved << "\n";
  std::cout << "fallback_entry_status=" << fallback_entry_status << "\n";
  std::cout << "fallback_entry_found=" << fallback_entry.found << "\n";
  std::cout << "fallback_entry_resolved=" << fallback_entry.resolved << "\n";
  std::cout << "direct_delta_cache_entry_count="
            << (direct_state.cache_entry_count - baseline.cache_entry_count)
            << "\n";
  std::cout << "direct_delta_cache_hit_count="
            << (direct_state.cache_hit_count - baseline.cache_hit_count)
            << "\n";
  std::cout << "direct_delta_cache_miss_count="
            << (direct_state.cache_miss_count - baseline.cache_miss_count)
            << "\n";
  std::cout << "direct_delta_live_dispatch_count="
            << (direct_state.live_dispatch_count - baseline.live_dispatch_count)
            << "\n";
  std::cout << "mixed_first_delta_cache_entry_count="
            << (mixed_first_state.cache_entry_count -
                direct_state.cache_entry_count)
            << "\n";
  std::cout << "mixed_first_delta_cache_miss_count="
            << (mixed_first_state.cache_miss_count -
                direct_state.cache_miss_count)
            << "\n";
  std::cout << "mixed_first_delta_slow_path_lookup_count="
            << (mixed_first_state.slow_path_lookup_count -
                direct_state.slow_path_lookup_count)
            << "\n";
  std::cout << "mixed_first_delta_live_dispatch_count="
            << (mixed_first_state.live_dispatch_count -
                direct_state.live_dispatch_count)
            << "\n";
  std::cout << "mixed_second_delta_cache_hit_count="
            << (mixed_second_state.cache_hit_count -
                mixed_first_state.cache_hit_count)
            << "\n";
  std::cout << "mixed_second_delta_live_dispatch_count="
            << (mixed_second_state.live_dispatch_count -
                mixed_first_state.live_dispatch_count)
            << "\n";
  std::cout << "fallback_first_delta_cache_entry_count="
            << (fallback_first_state.cache_entry_count -
                mixed_second_state.cache_entry_count)
            << "\n";
  std::cout << "fallback_first_delta_cache_miss_count="
            << (fallback_first_state.cache_miss_count -
                mixed_second_state.cache_miss_count)
            << "\n";
  std::cout << "fallback_first_delta_slow_path_lookup_count="
            << (fallback_first_state.slow_path_lookup_count -
                mixed_second_state.slow_path_lookup_count)
            << "\n";
  std::cout << "fallback_first_delta_strict_dispatch_error_count="
            << (fallback_first_state.strict_dispatch_error_count -
                mixed_second_state.strict_dispatch_error_count)
            << "\n";
  std::cout << "fallback_second_delta_cache_hit_count="
            << (fallback_second_state.cache_hit_count -
                fallback_first_state.cache_hit_count)
            << "\n";
  std::cout << "fallback_second_delta_strict_dispatch_error_count="
            << (fallback_second_state.strict_dispatch_error_count -
                fallback_first_state.strict_dispatch_error_count)
            << "\n";
  WriteLabeledMethodCacheState(std::cout, "baseline", baseline,
                               baseline_last_selector);
  WriteLabeledMethodCacheState(std::cout, "direct", direct_state,
                               direct_last_selector);
  WriteLabeledMethodCacheState(std::cout, "mixed_first_state",
                               mixed_first_state, mixed_first_last_selector);
  WriteLabeledMethodCacheState(std::cout, "mixed_second_state",
                               mixed_second_state, mixed_second_last_selector);
  WriteLabeledMethodCacheState(std::cout, "fallback_first_state",
                               fallback_first_state,
                               fallback_first_last_selector);
  WriteLabeledMethodCacheState(std::cout, "fallback_second_state",
                               fallback_second_state,
                               fallback_second_last_selector);

  const bool ok =
      baseline_status == 0 && direct_status == 0 && mixed_first_status == 0 &&
      mixed_second_status == 0 && fallback_first_status == 0 &&
      fallback_second_status == 0 && implicit_value == 3 &&
      explicit_value == 5 && mixed_first == 12 && mixed_second == 12 &&
      direct_state.cache_entry_count == baseline.cache_entry_count &&
      direct_state.cache_hit_count == baseline.cache_hit_count &&
      direct_state.cache_miss_count == baseline.cache_miss_count &&
      direct_state.slow_path_lookup_count == baseline.slow_path_lookup_count &&
      direct_state.live_dispatch_count == baseline.live_dispatch_count &&
      direct_state.strict_dispatch_error_count ==
          baseline.strict_dispatch_error_count &&
      mixed_first_state.cache_entry_count == direct_state.cache_entry_count &&
      mixed_first_state.cache_hit_count == direct_state.cache_hit_count + 1 &&
      mixed_first_state.cache_miss_count == direct_state.cache_miss_count &&
      mixed_first_state.slow_path_lookup_count ==
          direct_state.slow_path_lookup_count &&
      mixed_first_state.live_dispatch_count ==
          direct_state.live_dispatch_count + 1 &&
      mixed_first_state.strict_dispatch_error_count ==
          direct_state.strict_dispatch_error_count &&
      mixed_first_state.last_dispatch_used_cache == 1 &&
      mixed_first_state.last_dispatch_resolved_live_method == 1 &&
      mixed_first_state.last_dispatch_strict_error == 0 &&
      mixed_second_state.cache_entry_count ==
          mixed_first_state.cache_entry_count &&
      mixed_second_state.cache_hit_count ==
          mixed_first_state.cache_hit_count + 1 &&
      mixed_second_state.cache_miss_count ==
          mixed_first_state.cache_miss_count &&
      mixed_second_state.live_dispatch_count ==
          mixed_first_state.live_dispatch_count + 1 &&
      mixed_second_state.last_dispatch_used_cache == 1 &&
      mixed_second_state.last_dispatch_resolved_live_method == 1 &&
      mixed_second_state.last_dispatch_strict_error == 0 &&
      fallback_first == fallback_expected &&
      fallback_second == fallback_expected &&
      fallback_first_state.cache_entry_count ==
          mixed_second_state.cache_entry_count + 1 &&
      fallback_first_state.cache_miss_count ==
          mixed_second_state.cache_miss_count + 1 &&
      fallback_first_state.slow_path_lookup_count ==
          mixed_second_state.slow_path_lookup_count + 1 &&
      fallback_first_state.strict_dispatch_error_count ==
          mixed_second_state.strict_dispatch_error_count + 1 &&
      fallback_first_state.last_dispatch_used_cache == 0 &&
      fallback_first_state.last_dispatch_resolved_live_method == 0 &&
      fallback_first_state.last_dispatch_strict_error == 1 &&
      fallback_second_state.cache_entry_count ==
          fallback_first_state.cache_entry_count &&
      fallback_second_state.cache_hit_count ==
          fallback_first_state.cache_hit_count + 1 &&
      fallback_second_state.cache_miss_count ==
          fallback_first_state.cache_miss_count &&
      fallback_second_state.strict_dispatch_error_count ==
          fallback_first_state.strict_dispatch_error_count + 1 &&
      fallback_second_state.last_dispatch_used_cache == 1 &&
      fallback_second_state.last_dispatch_resolved_live_method == 0 &&
      fallback_second_state.last_dispatch_strict_error == 1 &&
      dynamic_entry_status == 0 && dynamic_entry.found == 1 &&
      dynamic_entry.resolved == 1 && fallback_entry_status == 0 &&
      fallback_entry.found == 1 && fallback_entry.resolved == 0;

  return ok ? 0 : 1;
}
