#include <iostream>
#include <string>

#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" int callImplicit(void);
extern "C" int callExplicit(void);
extern "C" int callMixed(void);

namespace {

constexpr long long kDispatchModulus = 2147483629LL;

long long ComputeSelectorScore(const char *selector) {
  if (selector == nullptr) {
    return 0;
  }
  long long selector_score = 0;
  long long index = 1;
  const unsigned char *cursor =
      reinterpret_cast<const unsigned char *>(selector);
  while (*cursor != 0U) {
    selector_score =
        (selector_score + (static_cast<long long>(*cursor) * index)) %
        kDispatchModulus;
    ++cursor;
    ++index;
  }
  return selector_score;
}

int ComputeFallbackDispatch(int receiver, const char *selector, int a0, int a1,
                            int a2, int a3) {
  long long value = 41;
  value += static_cast<long long>(receiver) * 97;
  value += static_cast<long long>(a0) * 7;
  value += static_cast<long long>(a1) * 11;
  value += static_cast<long long>(a2) * 13;
  value += static_cast<long long>(a3) * 17;
  value += ComputeSelectorScore(selector) * 19;
  value %= kDispatchModulus;
  if (value < 0) {
    value += kDispatchModulus;
  }
  return static_cast<int>(value);
}

void PrintState(const char *label,
                const objc3_runtime_method_cache_state_snapshot &snapshot,
                const std::string &last_selector,
                const std::string &last_fast_path_reason) {
  std::cout << label << "_cache_entry_count=" << snapshot.cache_entry_count
            << "\n";
  std::cout << label << "_cache_hit_count=" << snapshot.cache_hit_count << "\n";
  std::cout << label << "_cache_miss_count=" << snapshot.cache_miss_count
            << "\n";
  std::cout << label << "_slow_path_lookup_count="
            << snapshot.slow_path_lookup_count << "\n";
  std::cout << label << "_live_dispatch_count=" << snapshot.live_dispatch_count
            << "\n";
  std::cout << label << "_fallback_dispatch_count="
            << snapshot.fallback_dispatch_count << "\n";
  std::cout << label << "_fast_path_seed_count=" << snapshot.fast_path_seed_count
            << "\n";
  std::cout << label << "_fast_path_hit_count=" << snapshot.fast_path_hit_count
            << "\n";
  std::cout << label << "_last_dispatch_used_cache="
            << snapshot.last_dispatch_used_cache << "\n";
  std::cout << label << "_last_dispatch_used_fast_path="
            << snapshot.last_dispatch_used_fast_path << "\n";
  std::cout << label << "_last_dispatch_resolved_live_method="
            << snapshot.last_dispatch_resolved_live_method << "\n";
  std::cout << label << "_last_dispatch_fell_back="
            << snapshot.last_dispatch_fell_back << "\n";
  std::cout << label << "_last_selector=" << last_selector << "\n";
  std::cout << label << "_last_fast_path_reason=" << last_fast_path_reason
            << "\n";
}

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
  std::cout << label << "_objc_sealed_declared=" << snapshot.objc_sealed_declared
            << "\n";
  std::cout << label << "_selector=" << selector << "\n";
  std::cout << label << "_fast_path_reason=" << fast_path_reason << "\n";
}

void PrintDispatchState(const char *label,
                        const objc3_runtime_dispatch_state_snapshot &snapshot,
                        const std::string &last_selector,
                        const std::string &last_fast_path_reason,
                        const std::string &last_dispatch_path,
                        const std::string &last_implementation_kind,
                        const std::string &last_resolved_class_name) {
  std::cout << label << "_cache_entry_count=" << snapshot.cache_entry_count
            << "\n";
  std::cout << label << "_fast_path_seed_count=" << snapshot.fast_path_seed_count
            << "\n";
  std::cout << label << "_fast_path_hit_count=" << snapshot.fast_path_hit_count
            << "\n";
  std::cout << label << "_live_dispatch_count=" << snapshot.live_dispatch_count
            << "\n";
  std::cout << label << "_fallback_dispatch_count="
            << snapshot.fallback_dispatch_count << "\n";
  std::cout << label << "_last_resolved_parameter_count="
            << snapshot.last_resolved_parameter_count << "\n";
  std::cout << label << "_last_dispatch_used_cache="
            << snapshot.last_dispatch_used_cache << "\n";
  std::cout << label << "_last_dispatch_used_fast_path="
            << snapshot.last_dispatch_used_fast_path << "\n";
  std::cout << label << "_last_dispatch_resolved_live_method="
            << snapshot.last_dispatch_resolved_live_method << "\n";
  std::cout << label << "_last_dispatch_fell_back="
            << snapshot.last_dispatch_fell_back << "\n";
  std::cout << label << "_last_effective_direct_dispatch="
            << snapshot.last_effective_direct_dispatch << "\n";
  std::cout << label << "_last_used_builtin=" << snapshot.last_used_builtin
            << "\n";
  std::cout << label << "_last_selector=" << last_selector << "\n";
  std::cout << label << "_last_fast_path_reason=" << last_fast_path_reason
            << "\n";
  std::cout << label << "_last_dispatch_path=" << last_dispatch_path << "\n";
  std::cout << label << "_last_implementation_kind="
            << last_implementation_kind << "\n";
  std::cout << label << "_last_resolved_class_name="
            << last_resolved_class_name << "\n";
}

}  // namespace

int main() {
  objc3_runtime_method_cache_state_snapshot baseline{};
  objc3_runtime_method_cache_state_snapshot direct_state{};
  objc3_runtime_method_cache_state_snapshot mixed_first_state{};
  objc3_runtime_method_cache_state_snapshot mixed_second_state{};
  objc3_runtime_method_cache_state_snapshot fallback_first_state{};
  objc3_runtime_method_cache_state_snapshot fallback_second_state{};
  objc3_runtime_dispatch_state_snapshot mixed_first_dispatch_state{};
  objc3_runtime_dispatch_state_snapshot mixed_second_dispatch_state{};
  objc3_runtime_dispatch_state_snapshot fallback_first_dispatch_state{};
  objc3_runtime_dispatch_state_snapshot fallback_second_dispatch_state{};
  objc3_runtime_method_cache_entry_snapshot dynamic_entry{};
  objc3_runtime_method_cache_entry_snapshot explicit_entry{};
  objc3_runtime_method_cache_entry_snapshot fallback_entry{};
  std::string baseline_last_selector;
  std::string baseline_last_fast_path_reason;
  std::string direct_last_selector;
  std::string direct_last_fast_path_reason;
  std::string mixed_first_last_selector;
  std::string mixed_first_last_fast_path_reason;
  std::string mixed_second_last_selector;
  std::string mixed_second_last_fast_path_reason;
  std::string fallback_first_last_selector;
  std::string fallback_first_last_fast_path_reason;
  std::string fallback_second_last_selector;
  std::string fallback_second_last_fast_path_reason;
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
  std::string fallback_first_dispatch_last_selector;
  std::string fallback_first_dispatch_last_fast_path_reason;
  std::string fallback_first_dispatch_last_path;
  std::string fallback_first_dispatch_last_implementation_kind;
  std::string fallback_first_dispatch_last_resolved_class_name;
  std::string fallback_second_dispatch_last_selector;
  std::string fallback_second_dispatch_last_fast_path_reason;
  std::string fallback_second_dispatch_last_path;
  std::string fallback_second_dispatch_last_implementation_kind;
  std::string fallback_second_dispatch_last_resolved_class_name;
  std::string dynamic_entry_selector;
  std::string dynamic_entry_fast_path_reason;
  std::string explicit_entry_selector;
  std::string explicit_entry_fast_path_reason;
  std::string fallback_entry_selector;
  std::string fallback_entry_fast_path_reason;

  const int baseline_status =
      objc3_runtime_copy_method_cache_state_for_testing(&baseline);
  baseline_last_selector = baseline.last_selector != nullptr ? baseline.last_selector : "";
  baseline_last_fast_path_reason =
      baseline.last_fast_path_reason != nullptr ? baseline.last_fast_path_reason : "";
  const int dynamic_entry_status = objc3_runtime_copy_method_cache_entry_for_testing(
      1024, "dynamicEscape", &dynamic_entry);
  dynamic_entry_selector = dynamic_entry.selector != nullptr ? dynamic_entry.selector : "";
  dynamic_entry_fast_path_reason =
      dynamic_entry.fast_path_reason != nullptr ? dynamic_entry.fast_path_reason : "";
  const int explicit_entry_status = objc3_runtime_copy_method_cache_entry_for_testing(
      1024, "explicitDirect", &explicit_entry);
  explicit_entry_selector = explicit_entry.selector != nullptr ? explicit_entry.selector : "";
  explicit_entry_fast_path_reason =
      explicit_entry.fast_path_reason != nullptr ? explicit_entry.fast_path_reason : "";

  const int implicit_value = callImplicit();
  const int explicit_value = callExplicit();
  const int direct_status = objc3_runtime_copy_method_cache_state_for_testing(
      &direct_state);
  direct_last_selector = direct_state.last_selector != nullptr ? direct_state.last_selector : "";
  direct_last_fast_path_reason =
      direct_state.last_fast_path_reason != nullptr ? direct_state.last_fast_path_reason : "";

  const int mixed_first = callMixed();
  const int mixed_first_status =
      objc3_runtime_copy_method_cache_state_for_testing(&mixed_first_state);
  const int mixed_first_dispatch_state_status =
      objc3_runtime_copy_dispatch_state_for_testing(&mixed_first_dispatch_state);
  mixed_first_last_selector =
      mixed_first_state.last_selector != nullptr ? mixed_first_state.last_selector : "";
  mixed_first_last_fast_path_reason = mixed_first_state.last_fast_path_reason != nullptr
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
      objc3_runtime_copy_dispatch_state_for_testing(&mixed_second_dispatch_state);
  mixed_second_last_selector = mixed_second_state.last_selector != nullptr
                                   ? mixed_second_state.last_selector
                                   : "";
  mixed_second_last_fast_path_reason = mixed_second_state.last_fast_path_reason != nullptr
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

  const char *const fallback_selector = "missingDispatch:";
  const int fallback_expected =
      ComputeFallbackDispatch(1024, fallback_selector, 4, 5, 6, 7);
  const int fallback_first =
      objc3_runtime_dispatch_i32(1024, fallback_selector, 4, 5, 6, 7);
  const int fallback_first_status =
      objc3_runtime_copy_method_cache_state_for_testing(&fallback_first_state);
  const int fallback_first_dispatch_state_status =
      objc3_runtime_copy_dispatch_state_for_testing(
          &fallback_first_dispatch_state);
  fallback_first_last_selector = fallback_first_state.last_selector != nullptr
                                     ? fallback_first_state.last_selector
                                     : "";
  fallback_first_last_fast_path_reason =
      fallback_first_state.last_fast_path_reason != nullptr
          ? fallback_first_state.last_fast_path_reason
          : "";
  fallback_first_dispatch_last_selector =
      fallback_first_dispatch_state.last_selector != nullptr
          ? fallback_first_dispatch_state.last_selector
          : "";
  fallback_first_dispatch_last_fast_path_reason =
      fallback_first_dispatch_state.last_fast_path_reason != nullptr
          ? fallback_first_dispatch_state.last_fast_path_reason
          : "";
  fallback_first_dispatch_last_path =
      fallback_first_dispatch_state.last_dispatch_path != nullptr
          ? fallback_first_dispatch_state.last_dispatch_path
          : "";
  fallback_first_dispatch_last_implementation_kind =
      fallback_first_dispatch_state.last_implementation_kind != nullptr
          ? fallback_first_dispatch_state.last_implementation_kind
          : "";
  fallback_first_dispatch_last_resolved_class_name =
      fallback_first_dispatch_state.last_resolved_class_name != nullptr
          ? fallback_first_dispatch_state.last_resolved_class_name
          : "";

  const int fallback_second =
      objc3_runtime_dispatch_i32(1024, fallback_selector, 4, 5, 6, 7);
  const int fallback_second_status =
      objc3_runtime_copy_method_cache_state_for_testing(&fallback_second_state);
  const int fallback_second_dispatch_state_status =
      objc3_runtime_copy_dispatch_state_for_testing(
          &fallback_second_dispatch_state);
  fallback_second_last_selector = fallback_second_state.last_selector != nullptr
                                      ? fallback_second_state.last_selector
                                      : "";
  fallback_second_last_fast_path_reason =
      fallback_second_state.last_fast_path_reason != nullptr
          ? fallback_second_state.last_fast_path_reason
          : "";
  fallback_second_dispatch_last_selector =
      fallback_second_dispatch_state.last_selector != nullptr
          ? fallback_second_dispatch_state.last_selector
          : "";
  fallback_second_dispatch_last_fast_path_reason =
      fallback_second_dispatch_state.last_fast_path_reason != nullptr
          ? fallback_second_dispatch_state.last_fast_path_reason
          : "";
  fallback_second_dispatch_last_path =
      fallback_second_dispatch_state.last_dispatch_path != nullptr
          ? fallback_second_dispatch_state.last_dispatch_path
          : "";
  fallback_second_dispatch_last_implementation_kind =
      fallback_second_dispatch_state.last_implementation_kind != nullptr
          ? fallback_second_dispatch_state.last_implementation_kind
          : "";
  fallback_second_dispatch_last_resolved_class_name =
      fallback_second_dispatch_state.last_resolved_class_name != nullptr
          ? fallback_second_dispatch_state.last_resolved_class_name
          : "";

  const int fallback_entry_status = objc3_runtime_copy_method_cache_entry_for_testing(
      1024, fallback_selector, &fallback_entry);
  fallback_entry_selector = fallback_entry.selector != nullptr ? fallback_entry.selector : "";
  fallback_entry_fast_path_reason =
      fallback_entry.fast_path_reason != nullptr ? fallback_entry.fast_path_reason : "";

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
  std::cout << "fallback_expected=" << fallback_expected << "\n";
  std::cout << "fallback_first=" << fallback_first << "\n";
  std::cout << "fallback_first_status=" << fallback_first_status << "\n";
  std::cout << "fallback_first_dispatch_state_status="
            << fallback_first_dispatch_state_status << "\n";
  std::cout << "fallback_second=" << fallback_second << "\n";
  std::cout << "fallback_second_status=" << fallback_second_status << "\n";
  std::cout << "fallback_second_dispatch_state_status="
            << fallback_second_dispatch_state_status << "\n";
  std::cout << "fallback_entry_status=" << fallback_entry_status << "\n";
  std::cout << "direct_delta_cache_entry_count="
            << (direct_state.cache_entry_count - baseline.cache_entry_count) << "\n";
  std::cout << "direct_delta_cache_hit_count="
            << (direct_state.cache_hit_count - baseline.cache_hit_count) << "\n";
  std::cout << "direct_delta_fast_path_hit_count="
            << (direct_state.fast_path_hit_count - baseline.fast_path_hit_count)
            << "\n";
  std::cout << "direct_delta_live_dispatch_count="
            << (direct_state.live_dispatch_count - baseline.live_dispatch_count)
            << "\n";
  std::cout << "mixed_first_delta_cache_hit_count="
            << (mixed_first_state.cache_hit_count - direct_state.cache_hit_count)
            << "\n";
  std::cout << "mixed_first_delta_cache_miss_count="
            << (mixed_first_state.cache_miss_count - direct_state.cache_miss_count)
            << "\n";
  std::cout << "mixed_first_delta_slow_path_lookup_count="
            << (mixed_first_state.slow_path_lookup_count -
                direct_state.slow_path_lookup_count)
            << "\n";
  std::cout << "mixed_first_delta_fast_path_hit_count="
            << (mixed_first_state.fast_path_hit_count - direct_state.fast_path_hit_count)
            << "\n";
  std::cout << "mixed_first_delta_live_dispatch_count="
            << (mixed_first_state.live_dispatch_count - direct_state.live_dispatch_count)
            << "\n";
  std::cout << "mixed_second_delta_cache_hit_count="
            << (mixed_second_state.cache_hit_count - mixed_first_state.cache_hit_count)
            << "\n";
  std::cout << "mixed_second_delta_fast_path_hit_count="
            << (mixed_second_state.fast_path_hit_count - mixed_first_state.fast_path_hit_count)
            << "\n";
  std::cout << "mixed_second_delta_live_dispatch_count="
            << (mixed_second_state.live_dispatch_count - mixed_first_state.live_dispatch_count)
            << "\n";
  std::cout << "fallback_first_delta_cache_entry_count="
            << (fallback_first_state.cache_entry_count - mixed_second_state.cache_entry_count)
            << "\n";
  std::cout << "fallback_first_delta_cache_miss_count="
            << (fallback_first_state.cache_miss_count - mixed_second_state.cache_miss_count)
            << "\n";
  std::cout << "fallback_first_delta_slow_path_lookup_count="
            << (fallback_first_state.slow_path_lookup_count -
                mixed_second_state.slow_path_lookup_count)
            << "\n";
  std::cout << "fallback_first_delta_fallback_dispatch_count="
            << (fallback_first_state.fallback_dispatch_count -
                mixed_second_state.fallback_dispatch_count)
            << "\n";
  std::cout << "fallback_second_delta_cache_hit_count="
            << (fallback_second_state.cache_hit_count -
                fallback_first_state.cache_hit_count)
            << "\n";
  std::cout << "fallback_second_delta_fallback_dispatch_count="
            << (fallback_second_state.fallback_dispatch_count -
                fallback_first_state.fallback_dispatch_count)
            << "\n";

  PrintEntry("dynamic_entry", dynamic_entry, dynamic_entry_selector,
             dynamic_entry_fast_path_reason);
  PrintEntry("explicit_entry", explicit_entry, explicit_entry_selector,
             explicit_entry_fast_path_reason);
  PrintEntry("fallback_entry", fallback_entry, fallback_entry_selector,
             fallback_entry_fast_path_reason);
  PrintState("baseline", baseline, baseline_last_selector,
             baseline_last_fast_path_reason);
  PrintState("direct", direct_state, direct_last_selector,
             direct_last_fast_path_reason);
  PrintState("mixed_first_state", mixed_first_state, mixed_first_last_selector,
             mixed_first_last_fast_path_reason);
  PrintState("mixed_second_state", mixed_second_state,
             mixed_second_last_selector, mixed_second_last_fast_path_reason);
  PrintState("fallback_first_state", fallback_first_state,
             fallback_first_last_selector, fallback_first_last_fast_path_reason);
  PrintState("fallback_second_state", fallback_second_state,
             fallback_second_last_selector,
             fallback_second_last_fast_path_reason);
  PrintDispatchState("mixed_first_dispatch_state", mixed_first_dispatch_state,
                     mixed_first_dispatch_last_selector,
                     mixed_first_dispatch_last_fast_path_reason,
                     mixed_first_dispatch_last_path,
                     mixed_first_dispatch_last_implementation_kind,
                     mixed_first_dispatch_last_resolved_class_name);
  PrintDispatchState("mixed_second_dispatch_state", mixed_second_dispatch_state,
                     mixed_second_dispatch_last_selector,
                     mixed_second_dispatch_last_fast_path_reason,
                     mixed_second_dispatch_last_path,
                     mixed_second_dispatch_last_implementation_kind,
                     mixed_second_dispatch_last_resolved_class_name);
  PrintDispatchState("fallback_first_dispatch_state",
                     fallback_first_dispatch_state,
                     fallback_first_dispatch_last_selector,
                     fallback_first_dispatch_last_fast_path_reason,
                     fallback_first_dispatch_last_path,
                     fallback_first_dispatch_last_implementation_kind,
                     fallback_first_dispatch_last_resolved_class_name);
  PrintDispatchState("fallback_second_dispatch_state",
                     fallback_second_dispatch_state,
                     fallback_second_dispatch_last_selector,
                     fallback_second_dispatch_last_fast_path_reason,
                     fallback_second_dispatch_last_path,
                     fallback_second_dispatch_last_implementation_kind,
                     fallback_second_dispatch_last_resolved_class_name);

  const bool ok =
      baseline_status == 0 && dynamic_entry_status == 0 && explicit_entry_status == 0 &&
      direct_status == 0 && mixed_first_status == 0 && mixed_second_status == 0 &&
      mixed_first_dispatch_state_status == 0 &&
      mixed_second_dispatch_state_status == 0 &&
      fallback_first_status == 0 && fallback_second_status == 0 &&
      fallback_first_dispatch_state_status == 0 &&
      fallback_second_dispatch_state_status == 0 &&
      fallback_entry_status == 0 && implicit_value == 3 && explicit_value == 5 &&
      mixed_first == 12 && mixed_second == 12 &&
      baseline.cache_entry_count == 4 && baseline.fast_path_seed_count == 4 &&
      dynamic_entry.found == 1 && dynamic_entry.resolved == 1 &&
      dynamic_entry.fast_path_seeded == 1 && dynamic_entry.effective_direct_dispatch == 0 &&
      dynamic_entry.objc_final_declared == 1 && dynamic_entry.objc_sealed_declared == 1 &&
      dynamic_entry_fast_path_reason == "class-final" &&
      explicit_entry.found == 1 && explicit_entry.resolved == 1 &&
      explicit_entry.fast_path_seeded == 1 && explicit_entry.effective_direct_dispatch == 1 &&
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
      mixed_first_state.slow_path_lookup_count == direct_state.slow_path_lookup_count &&
      mixed_first_state.fast_path_hit_count == direct_state.fast_path_hit_count + 1 &&
      mixed_first_state.live_dispatch_count == direct_state.live_dispatch_count + 1 &&
      mixed_first_state.last_dispatch_used_cache == 1 &&
      mixed_first_state.last_dispatch_used_fast_path == 1 &&
      mixed_first_state.last_dispatch_resolved_live_method == 1 &&
      mixed_first_state.last_dispatch_fell_back == 0 &&
      mixed_first_last_selector == "dynamicEscape" &&
      mixed_first_last_fast_path_reason == "class-final" &&
      mixed_first_dispatch_last_path == "cache-hit-fast-path" &&
      mixed_first_dispatch_last_implementation_kind == "emitted-method-body" &&
      mixed_first_dispatch_state.last_effective_direct_dispatch == 0 &&
      mixed_first_dispatch_state.last_used_builtin == 0 &&
      mixed_first_dispatch_state.last_resolved_parameter_count == 0 &&
      mixed_first_dispatch_last_resolved_class_name == "PolicyBox" &&
      mixed_second_state.cache_entry_count == mixed_first_state.cache_entry_count &&
      mixed_second_state.cache_hit_count == mixed_first_state.cache_hit_count + 1 &&
      mixed_second_state.fast_path_hit_count == mixed_first_state.fast_path_hit_count + 1 &&
      mixed_second_state.live_dispatch_count == mixed_first_state.live_dispatch_count + 1 &&
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
      fallback_first == fallback_expected && fallback_second == fallback_expected &&
      fallback_first_state.cache_entry_count == mixed_second_state.cache_entry_count + 1 &&
      fallback_first_state.cache_miss_count == mixed_second_state.cache_miss_count + 1 &&
      fallback_first_state.slow_path_lookup_count ==
          mixed_second_state.slow_path_lookup_count + 1 &&
      fallback_first_state.fallback_dispatch_count ==
          mixed_second_state.fallback_dispatch_count + 1 &&
      fallback_first_state.last_dispatch_used_cache == 0 &&
      fallback_first_state.last_dispatch_used_fast_path == 0 &&
      fallback_first_state.last_dispatch_resolved_live_method == 0 &&
      fallback_first_state.last_dispatch_fell_back == 1 &&
      fallback_first_last_selector == "missingDispatch:" &&
      fallback_first_last_fast_path_reason.empty() &&
      fallback_first_dispatch_last_path == "slow-path-fallback" &&
      fallback_first_dispatch_last_implementation_kind == "fallback-formula" &&
      fallback_first_dispatch_state.last_effective_direct_dispatch == 0 &&
      fallback_first_dispatch_state.last_used_builtin == 0 &&
      fallback_first_dispatch_state.last_resolved_parameter_count == 0 &&
      fallback_first_dispatch_last_resolved_class_name.empty() &&
      fallback_second_state.cache_entry_count == fallback_first_state.cache_entry_count &&
      fallback_second_state.cache_hit_count == fallback_first_state.cache_hit_count + 1 &&
      fallback_second_state.fallback_dispatch_count ==
          fallback_first_state.fallback_dispatch_count + 1 &&
      fallback_second_state.last_dispatch_used_cache == 1 &&
      fallback_second_state.last_dispatch_used_fast_path == 0 &&
      fallback_second_state.last_dispatch_resolved_live_method == 0 &&
      fallback_second_state.last_dispatch_fell_back == 1 &&
      fallback_second_last_selector == "missingDispatch:" &&
      fallback_second_last_fast_path_reason.empty() &&
      fallback_second_dispatch_last_path == "cache-hit-fallback" &&
      fallback_second_dispatch_last_implementation_kind == "fallback-formula" &&
      fallback_second_dispatch_state.last_effective_direct_dispatch == 0 &&
      fallback_second_dispatch_state.last_used_builtin == 0 &&
      fallback_second_dispatch_state.last_resolved_parameter_count == 0 &&
      fallback_second_dispatch_last_resolved_class_name.empty() &&
      fallback_entry.found == 1 && fallback_entry.resolved == 0 &&
      fallback_entry.fast_path_seeded == 0 && fallback_entry_fast_path_reason.empty();

  return ok ? 0 : 1;
}
