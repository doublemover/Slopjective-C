#include <iostream>
#include <string>

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/dispatch_expectations.h"
#include "support/runtime_snapshot_text.h"

extern "C" int callImplicit(void);
extern "C" int callExplicit(void);
extern "C" int callMixed(void);

namespace {

using objc3c::runtime::probe::WriteLabeledFastPathMethodCacheState;

using objc3c::runtime::probe::ExpectedStrictDispatchErrorValue;

using objc3c::runtime::probe::WriteLabeledDispatchState;

void PrintEntry(const char *label,
                const objc3_runtime_method_cache_entry_snapshot &snapshot,
                const std::string &selector,
                const std::string &fast_path_reason) {
  std::cout << label << "_found=" << snapshot.found << "\n";
  std::cout << label << "_resolved=" << snapshot.resolved << "\n";
  std::cout << label << "_fast_path_seeded=" << snapshot.fast_path_seeded
            << "\n";
  std::cout << label << "_effective_direct_dispatch="
            << snapshot.effective_direct_dispatch << "\n";
  std::cout << label << "_objc_final_declared=" << snapshot.objc_final_declared
            << "\n";
  std::cout << label
            << "_objc_sealed_declared=" << snapshot.objc_sealed_declared
            << "\n";
  std::cout << label << "_selector=" << selector << "\n";
  std::cout << label << "_fast_path_reason=" << fast_path_reason << "\n";
}

} // namespace

int main() {
  objc3_runtime_method_cache_state_snapshot baseline{};
  objc3_runtime_method_cache_state_snapshot direct_state{};
  objc3_runtime_method_cache_state_snapshot mixed_first_state{};
  objc3_runtime_method_cache_state_snapshot mixed_second_state{};
  objc3_runtime_method_cache_state_snapshot strict_error_first_state{};
  objc3_runtime_method_cache_state_snapshot strict_error_second_state{};
  objc3_runtime_dispatch_state_snapshot mixed_first_dispatch_state{};
  objc3_runtime_dispatch_state_snapshot mixed_second_dispatch_state{};
  objc3_runtime_dispatch_state_snapshot strict_error_first_dispatch_state{};
  objc3_runtime_dispatch_state_snapshot strict_error_second_dispatch_state{};
  objc3_runtime_method_cache_entry_snapshot dynamic_entry{};
  objc3_runtime_method_cache_entry_snapshot explicit_entry{};
  objc3_runtime_method_cache_entry_snapshot strict_error_entry{};
  std::string baseline_last_selector;
  std::string baseline_last_fast_path_reason;
  std::string direct_last_selector;
  std::string direct_last_fast_path_reason;
  std::string mixed_first_last_selector;
  std::string mixed_first_last_fast_path_reason;
  std::string mixed_second_last_selector;
  std::string mixed_second_last_fast_path_reason;
  std::string strict_error_first_last_selector;
  std::string strict_error_first_last_fast_path_reason;
  std::string strict_error_second_last_selector;
  std::string strict_error_second_last_fast_path_reason;
  std::string mixed_first_dispatch_last_selector;
  std::string mixed_first_dispatch_last_fast_path_reason;
  std::string mixed_first_dispatch_last_path;
  std::string mixed_first_dispatch_last_implementation_kind;
  std::string mixed_first_dispatch_last_resolved_class_name;
  std::string mixed_second_dispatch_last_selector;
  std::string mixed_second_dispatch_last_fast_path_reason;
  std::string mixed_second_dispatch_last_path;
  std::string mixed_second_dispatch_last_implementation_kind;
  std::string mixed_second_dispatch_last_resolved_class_name;
  std::string strict_error_first_dispatch_last_selector;
  std::string strict_error_first_dispatch_last_fast_path_reason;
  std::string strict_error_first_dispatch_last_path;
  std::string strict_error_first_dispatch_last_implementation_kind;
  std::string strict_error_first_dispatch_last_resolved_class_name;
  std::string strict_error_second_dispatch_last_selector;
  std::string strict_error_second_dispatch_last_fast_path_reason;
  std::string strict_error_second_dispatch_last_path;
  std::string strict_error_second_dispatch_last_implementation_kind;
  std::string strict_error_second_dispatch_last_resolved_class_name;
  std::string dynamic_entry_selector;
  std::string dynamic_entry_fast_path_reason;
  std::string explicit_entry_selector;
  std::string explicit_entry_fast_path_reason;
  std::string strict_error_entry_selector;
  std::string strict_error_entry_fast_path_reason;

  const int baseline_status =
      objc3_runtime_copy_method_cache_state_for_testing(&baseline);
  baseline_last_selector =
      baseline.last_selector != nullptr ? baseline.last_selector : "";
  baseline_last_fast_path_reason = baseline.last_fast_path_reason != nullptr
                                       ? baseline.last_fast_path_reason
                                       : "";
  const int dynamic_entry_status =
      objc3_runtime_copy_method_cache_entry_for_testing(1024, "dynamicEscape",
                                                        &dynamic_entry);
  dynamic_entry_selector =
      dynamic_entry.selector != nullptr ? dynamic_entry.selector : "";
  dynamic_entry_fast_path_reason = dynamic_entry.fast_path_reason != nullptr
                                       ? dynamic_entry.fast_path_reason
                                       : "";
  const int explicit_entry_status =
      objc3_runtime_copy_method_cache_entry_for_testing(1024, "explicitDirect",
                                                        &explicit_entry);
  explicit_entry_selector =
      explicit_entry.selector != nullptr ? explicit_entry.selector : "";
  explicit_entry_fast_path_reason = explicit_entry.fast_path_reason != nullptr
                                        ? explicit_entry.fast_path_reason
                                        : "";

  const int implicit_value = callImplicit();
  const int explicit_value = callExplicit();
  const int direct_status =
      objc3_runtime_copy_method_cache_state_for_testing(&direct_state);
  direct_last_selector =
      direct_state.last_selector != nullptr ? direct_state.last_selector : "";
  direct_last_fast_path_reason = direct_state.last_fast_path_reason != nullptr
                                     ? direct_state.last_fast_path_reason
                                     : "";

  const int mixed_first = callMixed();
  const int mixed_first_status =
      objc3_runtime_copy_method_cache_state_for_testing(&mixed_first_state);
  const int mixed_first_dispatch_state_status =
      objc3_runtime_copy_dispatch_state_for_testing(
          &mixed_first_dispatch_state);
  mixed_first_last_selector = mixed_first_state.last_selector != nullptr
                                  ? mixed_first_state.last_selector
                                  : "";
  mixed_first_last_fast_path_reason =
      mixed_first_state.last_fast_path_reason != nullptr
          ? mixed_first_state.last_fast_path_reason
          : "";
  mixed_first_dispatch_last_selector =
      mixed_first_dispatch_state.last_selector != nullptr
          ? mixed_first_dispatch_state.last_selector
          : "";
  mixed_first_dispatch_last_fast_path_reason =
      mixed_first_dispatch_state.last_fast_path_reason != nullptr
          ? mixed_first_dispatch_state.last_fast_path_reason
          : "";
  mixed_first_dispatch_last_path =
      mixed_first_dispatch_state.last_dispatch_path != nullptr
          ? mixed_first_dispatch_state.last_dispatch_path
          : "";
  mixed_first_dispatch_last_implementation_kind =
      mixed_first_dispatch_state.last_implementation_kind != nullptr
          ? mixed_first_dispatch_state.last_implementation_kind
          : "";
  mixed_first_dispatch_last_resolved_class_name =
      mixed_first_dispatch_state.last_resolved_class_name != nullptr
          ? mixed_first_dispatch_state.last_resolved_class_name
          : "";

  const int mixed_second = callMixed();
  const int mixed_second_status =
      objc3_runtime_copy_method_cache_state_for_testing(&mixed_second_state);
  const int mixed_second_dispatch_state_status =
      objc3_runtime_copy_dispatch_state_for_testing(
          &mixed_second_dispatch_state);
  mixed_second_last_selector = mixed_second_state.last_selector != nullptr
                                   ? mixed_second_state.last_selector
                                   : "";
  mixed_second_last_fast_path_reason =
      mixed_second_state.last_fast_path_reason != nullptr
          ? mixed_second_state.last_fast_path_reason
          : "";
  mixed_second_dispatch_last_selector =
      mixed_second_dispatch_state.last_selector != nullptr
          ? mixed_second_dispatch_state.last_selector
          : "";
  mixed_second_dispatch_last_fast_path_reason =
      mixed_second_dispatch_state.last_fast_path_reason != nullptr
          ? mixed_second_dispatch_state.last_fast_path_reason
          : "";
  mixed_second_dispatch_last_path =
      mixed_second_dispatch_state.last_dispatch_path != nullptr
          ? mixed_second_dispatch_state.last_dispatch_path
          : "";
  mixed_second_dispatch_last_implementation_kind =
      mixed_second_dispatch_state.last_implementation_kind != nullptr
          ? mixed_second_dispatch_state.last_implementation_kind
          : "";
  mixed_second_dispatch_last_resolved_class_name =
      mixed_second_dispatch_state.last_resolved_class_name != nullptr
          ? mixed_second_dispatch_state.last_resolved_class_name
          : "";

  const char *const strict_error_selector = "missingDispatch:";
  const int strict_error_expected =
      ExpectedStrictDispatchErrorValue(1024, strict_error_selector, 4, 5, 6, 7);
  const int strict_error_first =
      objc3_runtime_dispatch_i32(1024, strict_error_selector, 4, 5, 6, 7);
  const int strict_error_first_status =
      objc3_runtime_copy_method_cache_state_for_testing(&strict_error_first_state);
  const int strict_error_first_dispatch_state_status =
      objc3_runtime_copy_dispatch_state_for_testing(
          &strict_error_first_dispatch_state);
  strict_error_first_last_selector = strict_error_first_state.last_selector != nullptr
                                     ? strict_error_first_state.last_selector
                                     : "";
  strict_error_first_last_fast_path_reason =
      strict_error_first_state.last_fast_path_reason != nullptr
          ? strict_error_first_state.last_fast_path_reason
          : "";
  strict_error_first_dispatch_last_selector =
      strict_error_first_dispatch_state.last_selector != nullptr
          ? strict_error_first_dispatch_state.last_selector
          : "";
  strict_error_first_dispatch_last_fast_path_reason =
      strict_error_first_dispatch_state.last_fast_path_reason != nullptr
          ? strict_error_first_dispatch_state.last_fast_path_reason
          : "";
  strict_error_first_dispatch_last_path =
      strict_error_first_dispatch_state.last_dispatch_path != nullptr
          ? strict_error_first_dispatch_state.last_dispatch_path
          : "";
  strict_error_first_dispatch_last_implementation_kind =
      strict_error_first_dispatch_state.last_implementation_kind != nullptr
          ? strict_error_first_dispatch_state.last_implementation_kind
          : "";
  strict_error_first_dispatch_last_resolved_class_name =
      strict_error_first_dispatch_state.last_resolved_class_name != nullptr
          ? strict_error_first_dispatch_state.last_resolved_class_name
          : "";

  const int strict_error_second =
      objc3_runtime_dispatch_i32(1024, strict_error_selector, 4, 5, 6, 7);
  const int strict_error_second_status =
      objc3_runtime_copy_method_cache_state_for_testing(&strict_error_second_state);
  const int strict_error_second_dispatch_state_status =
      objc3_runtime_copy_dispatch_state_for_testing(
          &strict_error_second_dispatch_state);
  strict_error_second_last_selector = strict_error_second_state.last_selector != nullptr
                                      ? strict_error_second_state.last_selector
                                      : "";
  strict_error_second_last_fast_path_reason =
      strict_error_second_state.last_fast_path_reason != nullptr
          ? strict_error_second_state.last_fast_path_reason
          : "";
  strict_error_second_dispatch_last_selector =
      strict_error_second_dispatch_state.last_selector != nullptr
          ? strict_error_second_dispatch_state.last_selector
          : "";
  strict_error_second_dispatch_last_fast_path_reason =
      strict_error_second_dispatch_state.last_fast_path_reason != nullptr
          ? strict_error_second_dispatch_state.last_fast_path_reason
          : "";
  strict_error_second_dispatch_last_path =
      strict_error_second_dispatch_state.last_dispatch_path != nullptr
          ? strict_error_second_dispatch_state.last_dispatch_path
          : "";
  strict_error_second_dispatch_last_implementation_kind =
      strict_error_second_dispatch_state.last_implementation_kind != nullptr
          ? strict_error_second_dispatch_state.last_implementation_kind
          : "";
  strict_error_second_dispatch_last_resolved_class_name =
      strict_error_second_dispatch_state.last_resolved_class_name != nullptr
          ? strict_error_second_dispatch_state.last_resolved_class_name
          : "";

  const int strict_error_entry_status =
      objc3_runtime_copy_method_cache_entry_for_testing(1024, strict_error_selector,
                                                        &strict_error_entry);
  strict_error_entry_selector =
      strict_error_entry.selector != nullptr ? strict_error_entry.selector : "";
  strict_error_entry_fast_path_reason = strict_error_entry.fast_path_reason != nullptr
                                        ? strict_error_entry.fast_path_reason
                                        : "";

  std::cout << "baseline_status=" << baseline_status << "\n";
  std::cout << "dynamic_entry_status=" << dynamic_entry_status << "\n";
  std::cout << "explicit_entry_status=" << explicit_entry_status << "\n";
  std::cout << "implicit_value=" << implicit_value << "\n";
  std::cout << "explicit_value=" << explicit_value << "\n";
  std::cout << "direct_status=" << direct_status << "\n";
  std::cout << "mixed_first=" << mixed_first << "\n";
  std::cout << "mixed_first_status=" << mixed_first_status << "\n";
  std::cout << "mixed_first_dispatch_state_status="
            << mixed_first_dispatch_state_status << "\n";
  std::cout << "mixed_second=" << mixed_second << "\n";
  std::cout << "mixed_second_status=" << mixed_second_status << "\n";
  std::cout << "mixed_second_dispatch_state_status="
            << mixed_second_dispatch_state_status << "\n";
  std::cout << "strict_error_expected=" << strict_error_expected << "\n";
  std::cout << "strict_error_first=" << strict_error_first << "\n";
  std::cout << "strict_error_first_status=" << strict_error_first_status << "\n";
  std::cout << "strict_error_first_dispatch_state_status="
            << strict_error_first_dispatch_state_status << "\n";
  std::cout << "strict_error_second=" << strict_error_second << "\n";
  std::cout << "strict_error_second_status=" << strict_error_second_status << "\n";
  std::cout << "strict_error_second_dispatch_state_status="
            << strict_error_second_dispatch_state_status << "\n";
  std::cout << "strict_error_entry_status=" << strict_error_entry_status << "\n";
  std::cout << "direct_delta_cache_entry_count="
            << (direct_state.cache_entry_count - baseline.cache_entry_count)
            << "\n";
  std::cout << "direct_delta_cache_hit_count="
            << (direct_state.cache_hit_count - baseline.cache_hit_count)
            << "\n";
  std::cout << "direct_delta_fast_path_hit_count="
            << (direct_state.fast_path_hit_count - baseline.fast_path_hit_count)
            << "\n";
  std::cout << "direct_delta_live_dispatch_count="
            << (direct_state.live_dispatch_count - baseline.live_dispatch_count)
            << "\n";
  std::cout << "mixed_first_delta_cache_hit_count="
            << (mixed_first_state.cache_hit_count -
                direct_state.cache_hit_count)
            << "\n";
  std::cout << "mixed_first_delta_cache_miss_count="
            << (mixed_first_state.cache_miss_count -
                direct_state.cache_miss_count)
            << "\n";
  std::cout << "mixed_first_delta_slow_path_lookup_count="
            << (mixed_first_state.slow_path_lookup_count -
                direct_state.slow_path_lookup_count)
            << "\n";
  std::cout << "mixed_first_delta_fast_path_hit_count="
            << (mixed_first_state.fast_path_hit_count -
                direct_state.fast_path_hit_count)
            << "\n";
  std::cout << "mixed_first_delta_live_dispatch_count="
            << (mixed_first_state.live_dispatch_count -
                direct_state.live_dispatch_count)
            << "\n";
  std::cout << "mixed_second_delta_cache_hit_count="
            << (mixed_second_state.cache_hit_count -
                mixed_first_state.cache_hit_count)
            << "\n";
  std::cout << "mixed_second_delta_fast_path_hit_count="
            << (mixed_second_state.fast_path_hit_count -
                mixed_first_state.fast_path_hit_count)
            << "\n";
  std::cout << "mixed_second_delta_live_dispatch_count="
            << (mixed_second_state.live_dispatch_count -
                mixed_first_state.live_dispatch_count)
            << "\n";
  std::cout << "strict_error_first_delta_cache_entry_count="
            << (strict_error_first_state.cache_entry_count -
                mixed_second_state.cache_entry_count)
            << "\n";
  std::cout << "strict_error_first_delta_cache_miss_count="
            << (strict_error_first_state.cache_miss_count -
                mixed_second_state.cache_miss_count)
            << "\n";
  std::cout << "strict_error_first_delta_slow_path_lookup_count="
            << (strict_error_first_state.slow_path_lookup_count -
                mixed_second_state.slow_path_lookup_count)
            << "\n";
  std::cout << "strict_error_first_delta_strict_dispatch_error_count="
            << (strict_error_first_state.strict_dispatch_error_count -
                mixed_second_state.strict_dispatch_error_count)
            << "\n";
  std::cout << "strict_error_second_delta_cache_hit_count="
            << (strict_error_second_state.cache_hit_count -
                strict_error_first_state.cache_hit_count)
            << "\n";
  std::cout << "strict_error_second_delta_strict_dispatch_error_count="
            << (strict_error_second_state.strict_dispatch_error_count -
                strict_error_first_state.strict_dispatch_error_count)
            << "\n";

  PrintEntry("dynamic_entry", dynamic_entry, dynamic_entry_selector,
             dynamic_entry_fast_path_reason);
  PrintEntry("explicit_entry", explicit_entry, explicit_entry_selector,
             explicit_entry_fast_path_reason);
  PrintEntry("strict_error_entry", strict_error_entry, strict_error_entry_selector,
             strict_error_entry_fast_path_reason);
  WriteLabeledFastPathMethodCacheState(std::cout, "baseline", baseline,
                                       baseline_last_selector,
                                       baseline_last_fast_path_reason);
  WriteLabeledFastPathMethodCacheState(std::cout, "direct", direct_state,
                                       direct_last_selector,
                                       direct_last_fast_path_reason);
  WriteLabeledFastPathMethodCacheState(
      std::cout, "mixed_first_state", mixed_first_state,
      mixed_first_last_selector, mixed_first_last_fast_path_reason);
  WriteLabeledFastPathMethodCacheState(
      std::cout, "mixed_second_state", mixed_second_state,
      mixed_second_last_selector, mixed_second_last_fast_path_reason);
  WriteLabeledFastPathMethodCacheState(
      std::cout, "strict_error_first_state", strict_error_first_state,
      strict_error_first_last_selector, strict_error_first_last_fast_path_reason);
  WriteLabeledFastPathMethodCacheState(
      std::cout, "strict_error_second_state", strict_error_second_state,
      strict_error_second_last_selector, strict_error_second_last_fast_path_reason);
  WriteLabeledDispatchState(std::cout, "mixed_first_dispatch_state",
                            mixed_first_dispatch_state,
                            mixed_first_dispatch_last_selector,
                            mixed_first_dispatch_last_fast_path_reason,
                            mixed_first_dispatch_last_path,
                            mixed_first_dispatch_last_implementation_kind,
                            mixed_first_dispatch_last_resolved_class_name);
  WriteLabeledDispatchState(std::cout, "mixed_second_dispatch_state",
                            mixed_second_dispatch_state,
                            mixed_second_dispatch_last_selector,
                            mixed_second_dispatch_last_fast_path_reason,
                            mixed_second_dispatch_last_path,
                            mixed_second_dispatch_last_implementation_kind,
                            mixed_second_dispatch_last_resolved_class_name);
  WriteLabeledDispatchState(std::cout, "strict_error_first_dispatch_state",
                            strict_error_first_dispatch_state,
                            strict_error_first_dispatch_last_selector,
                            strict_error_first_dispatch_last_fast_path_reason,
                            strict_error_first_dispatch_last_path,
                            strict_error_first_dispatch_last_implementation_kind,
                            strict_error_first_dispatch_last_resolved_class_name);
  WriteLabeledDispatchState(std::cout, "strict_error_second_dispatch_state",
                            strict_error_second_dispatch_state,
                            strict_error_second_dispatch_last_selector,
                            strict_error_second_dispatch_last_fast_path_reason,
                            strict_error_second_dispatch_last_path,
                            strict_error_second_dispatch_last_implementation_kind,
                            strict_error_second_dispatch_last_resolved_class_name);

  const bool ok =
      baseline_status == 0 && dynamic_entry_status == 0 &&
      explicit_entry_status == 0 && direct_status == 0 &&
      mixed_first_status == 0 && mixed_second_status == 0 &&
      mixed_first_dispatch_state_status == 0 &&
      mixed_second_dispatch_state_status == 0 && strict_error_first_status == 0 &&
      strict_error_second_status == 0 &&
      strict_error_first_dispatch_state_status == 0 &&
      strict_error_second_dispatch_state_status == 0 &&
      strict_error_entry_status == 0 && implicit_value == 3 &&
      explicit_value == 5 && mixed_first == 12 && mixed_second == 12 &&
      baseline.cache_entry_count == 4 && baseline.fast_path_seed_count == 4 &&
      dynamic_entry.found == 1 && dynamic_entry.resolved == 1 &&
      dynamic_entry.fast_path_seeded == 1 &&
      dynamic_entry.effective_direct_dispatch == 0 &&
      dynamic_entry.objc_final_declared == 1 &&
      dynamic_entry.objc_sealed_declared == 1 &&
      dynamic_entry_fast_path_reason == "class-final" &&
      explicit_entry.found == 1 && explicit_entry.resolved == 1 &&
      explicit_entry.fast_path_seeded == 1 &&
      explicit_entry.effective_direct_dispatch == 1 &&
      explicit_entry_fast_path_reason == "direct" &&
      direct_state.cache_entry_count == baseline.cache_entry_count &&
      direct_state.cache_hit_count == baseline.cache_hit_count &&
      direct_state.cache_miss_count == baseline.cache_miss_count &&
      direct_state.slow_path_lookup_count == baseline.slow_path_lookup_count &&
      direct_state.fast_path_hit_count == baseline.fast_path_hit_count &&
      direct_state.live_dispatch_count == baseline.live_dispatch_count &&
      mixed_first_state.cache_entry_count == direct_state.cache_entry_count &&
      mixed_first_state.cache_hit_count == direct_state.cache_hit_count + 1 &&
      mixed_first_state.cache_miss_count == direct_state.cache_miss_count &&
      mixed_first_state.slow_path_lookup_count ==
          direct_state.slow_path_lookup_count &&
      mixed_first_state.fast_path_hit_count ==
          direct_state.fast_path_hit_count + 1 &&
      mixed_first_state.live_dispatch_count ==
          direct_state.live_dispatch_count + 1 &&
      mixed_first_state.last_dispatch_used_cache == 1 &&
      mixed_first_state.last_dispatch_used_fast_path == 1 &&
      mixed_first_state.last_dispatch_resolved_live_method == 1 &&
      mixed_first_state.last_dispatch_strict_error == 0 &&
      mixed_first_last_selector == "dynamicEscape" &&
      mixed_first_last_fast_path_reason == "class-final" &&
      mixed_first_dispatch_last_path == "cache-hit-fast-path" &&
      mixed_first_dispatch_last_implementation_kind == "emitted-method-body" &&
      mixed_first_dispatch_state.last_effective_direct_dispatch == 0 &&
      mixed_first_dispatch_state.last_used_builtin == 0 &&
      mixed_first_dispatch_state.last_resolved_parameter_count == 0 &&
      mixed_first_dispatch_last_resolved_class_name == "PolicyBox" &&
      mixed_second_state.cache_entry_count ==
          mixed_first_state.cache_entry_count &&
      mixed_second_state.cache_hit_count ==
          mixed_first_state.cache_hit_count + 1 &&
      mixed_second_state.fast_path_hit_count ==
          mixed_first_state.fast_path_hit_count + 1 &&
      mixed_second_state.live_dispatch_count ==
          mixed_first_state.live_dispatch_count + 1 &&
      mixed_second_state.last_dispatch_used_cache == 1 &&
      mixed_second_state.last_dispatch_used_fast_path == 1 &&
      mixed_second_last_selector == "dynamicEscape" &&
      mixed_second_last_fast_path_reason == "class-final" &&
      mixed_second_dispatch_last_path == "cache-hit-fast-path" &&
      mixed_second_dispatch_last_implementation_kind == "emitted-method-body" &&
      mixed_second_dispatch_state.last_effective_direct_dispatch == 0 &&
      mixed_second_dispatch_state.last_used_builtin == 0 &&
      mixed_second_dispatch_state.last_resolved_parameter_count == 0 &&
      mixed_second_dispatch_last_resolved_class_name == "PolicyBox" &&
      strict_error_first == strict_error_expected &&
      strict_error_second == strict_error_expected &&
      strict_error_first_state.cache_entry_count ==
          mixed_second_state.cache_entry_count + 1 &&
      strict_error_first_state.cache_miss_count ==
          mixed_second_state.cache_miss_count + 1 &&
      strict_error_first_state.slow_path_lookup_count ==
          mixed_second_state.slow_path_lookup_count + 1 &&
      strict_error_first_state.strict_dispatch_error_count ==
          mixed_second_state.strict_dispatch_error_count + 1 &&
      strict_error_first_state.last_dispatch_used_cache == 0 &&
      strict_error_first_state.last_dispatch_used_fast_path == 0 &&
      strict_error_first_state.last_dispatch_resolved_live_method == 0 &&
      strict_error_first_state.last_dispatch_strict_error == 1 &&
      strict_error_first_last_selector == "missingDispatch:" &&
      strict_error_first_last_fast_path_reason.empty() &&
      strict_error_first_dispatch_last_path == "slow-path-error" &&
      strict_error_first_dispatch_last_implementation_kind ==
          "strict-dispatch-error" &&
      strict_error_first_dispatch_state.last_effective_direct_dispatch == 0 &&
      strict_error_first_dispatch_state.last_used_builtin == 0 &&
      strict_error_first_dispatch_state.last_resolved_parameter_count == 0 &&
      strict_error_first_dispatch_last_resolved_class_name.empty() &&
      strict_error_second_state.cache_entry_count ==
          strict_error_first_state.cache_entry_count &&
      strict_error_second_state.cache_hit_count ==
          strict_error_first_state.cache_hit_count + 1 &&
      strict_error_second_state.strict_dispatch_error_count ==
          strict_error_first_state.strict_dispatch_error_count + 1 &&
      strict_error_second_state.last_dispatch_used_cache == 1 &&
      strict_error_second_state.last_dispatch_used_fast_path == 0 &&
      strict_error_second_state.last_dispatch_resolved_live_method == 0 &&
      strict_error_second_state.last_dispatch_strict_error == 1 &&
      strict_error_second_last_selector == "missingDispatch:" &&
      strict_error_second_last_fast_path_reason.empty() &&
      strict_error_second_dispatch_last_path == "cache-hit-error" &&
      strict_error_second_dispatch_last_implementation_kind ==
          "strict-dispatch-error" &&
      strict_error_second_dispatch_state.last_effective_direct_dispatch == 0 &&
      strict_error_second_dispatch_state.last_used_builtin == 0 &&
      strict_error_second_dispatch_state.last_resolved_parameter_count == 0 &&
      strict_error_second_dispatch_last_resolved_class_name.empty() &&
      strict_error_entry.found == 1 && strict_error_entry.resolved == 0 &&
      strict_error_entry.fast_path_seeded == 0 &&
      strict_error_entry_fast_path_reason.empty();

  return ok ? 0 : 1;
}
