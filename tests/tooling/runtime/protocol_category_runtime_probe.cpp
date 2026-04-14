#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_stabilizers.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>
#include <string>

namespace {

using objc3c::runtime::probe::PrintMethodCacheEntryWithProbeCounts;
using objc3c::runtime::probe::PrintMethodCacheStateProtocolCategory;
using objc3c::runtime::probe::PrintRegistrationStateBasic;
using objc3c::runtime::probe::PrintSelectorTableStateBasic;

using objc3c::runtime::probe::StabilizeMethodCacheEntry;
using objc3c::runtime::probe::StabilizeMethodCacheState;
using objc3c::runtime::probe::StabilizeNullableCString;
using objc3c::runtime::probe::StabilizeRegistrationState;
using objc3c::runtime::probe::StabilizeSelectorTableState;

using objc3c::runtime::probe::PrintJsonStringOrNull;

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


}  // namespace

int main() {
  objc3_runtime_registration_state_snapshot registration_state{};
  objc3_runtime_selector_lookup_table_state_snapshot selector_table_state{};
  objc3_runtime_method_cache_state_snapshot instance_first_state{};
  objc3_runtime_method_cache_state_snapshot instance_second_state{};
  objc3_runtime_method_cache_state_snapshot class_self_state{};
  objc3_runtime_method_cache_state_snapshot known_class_state{};
  objc3_runtime_method_cache_state_snapshot fallback_first_state{};
  objc3_runtime_method_cache_state_snapshot fallback_second_state{};
  objc3_runtime_method_cache_entry_snapshot instance_entry{};
  objc3_runtime_method_cache_entry_snapshot class_entry{};
  objc3_runtime_method_cache_entry_snapshot fallback_entry{};

  (void)objc3_runtime_copy_registration_state_for_testing(&registration_state);
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &selector_table_state);
  std::string registration_module_storage;
  std::string registration_identity_storage;
  std::string selector_table_last_storage;
  std::string instance_first_selector_storage;
  std::string instance_first_class_storage;
  std::string instance_first_owner_storage;
  std::string instance_second_selector_storage;
  std::string instance_second_class_storage;
  std::string instance_second_owner_storage;
  std::string class_self_selector_storage;
  std::string class_self_class_storage;
  std::string class_self_owner_storage;
  std::string known_class_selector_storage;
  std::string known_class_class_storage;
  std::string known_class_owner_storage;
  std::string fallback_first_selector_storage;
  std::string fallback_first_class_storage;
  std::string fallback_first_owner_storage;
  std::string fallback_second_selector_storage;
  std::string fallback_second_class_storage;
  std::string fallback_second_owner_storage;
  std::string instance_entry_selector_storage;
  std::string instance_entry_class_storage;
  std::string instance_entry_owner_storage;
  std::string class_entry_selector_storage;
  std::string class_entry_class_storage;
  std::string class_entry_owner_storage;
  std::string fallback_entry_selector_storage;
  std::string fallback_entry_class_storage;
  std::string fallback_entry_owner_storage;

  StabilizeRegistrationState(registration_state, registration_module_storage,
                             registration_identity_storage);
  StabilizeSelectorTableState(selector_table_state, selector_table_last_storage);
  const int instance_first =
      objc3_runtime_dispatch_i32(1025, "tracedValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&instance_first_state);
  StabilizeMethodCacheState(instance_first_state, instance_first_selector_storage,
                            instance_first_class_storage,
                            instance_first_owner_storage);
  const int instance_second =
      objc3_runtime_dispatch_i32(1025, "tracedValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&instance_second_state);
  StabilizeMethodCacheState(instance_second_state,
                            instance_second_selector_storage,
                            instance_second_class_storage,
                            instance_second_owner_storage);
  const int class_self =
      objc3_runtime_dispatch_i32(1026, "tracerClassValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&class_self_state);
  StabilizeMethodCacheState(class_self_state, class_self_selector_storage,
                            class_self_class_storage, class_self_owner_storage);
  const int known_class =
      objc3_runtime_dispatch_i32(1024, "tracerClassValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&known_class_state);
  StabilizeMethodCacheState(known_class_state, known_class_selector_storage,
                            known_class_class_storage,
                            known_class_owner_storage);
  const char *const fallback_selector = "protocolDeclaredOnly";
  const int fallback_first =
      objc3_runtime_dispatch_i32(1025, fallback_selector, 0, 0, 0, 0);
  const int fallback_expected =
      ComputeFallbackDispatch(1025, fallback_selector, 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&fallback_first_state);
  StabilizeMethodCacheState(fallback_first_state, fallback_first_selector_storage,
                            fallback_first_class_storage,
                            fallback_first_owner_storage);
  const int fallback_second =
      objc3_runtime_dispatch_i32(1025, fallback_selector, 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&fallback_second_state);
  StabilizeMethodCacheState(
      fallback_second_state, fallback_second_selector_storage,
      fallback_second_class_storage, fallback_second_owner_storage);

  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      1025, "tracedValue", &instance_entry);
  StabilizeMethodCacheEntry(instance_entry, instance_entry_selector_storage,
                            instance_entry_class_storage,
                            instance_entry_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      1024, "tracerClassValue", &class_entry);
  StabilizeMethodCacheEntry(class_entry, class_entry_selector_storage,
                            class_entry_class_storage, class_entry_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      1025, fallback_selector, &fallback_entry);
  StabilizeMethodCacheEntry(fallback_entry, fallback_entry_selector_storage,
                            fallback_entry_class_storage,
                            fallback_entry_owner_storage);

  std::printf("{");
  std::printf("\"registration_state\":");
  PrintRegistrationStateBasic(registration_state);
  std::printf(",\"selector_table_state\":");
  PrintSelectorTableStateBasic(selector_table_state);
  std::printf(",\"instance_first\":%d,", instance_first);
  std::printf("\"instance_second\":%d,", instance_second);
  std::printf("\"class_self\":%d,", class_self);
  std::printf("\"known_class\":%d,", known_class);
  std::printf("\"fallback_first\":%d,", fallback_first);
  std::printf("\"fallback_second\":%d,", fallback_second);
  std::printf("\"fallback_expected\":%d,", fallback_expected);
  std::printf("\"instance_first_state\":");
  PrintMethodCacheStateProtocolCategory(instance_first_state);
  std::printf(",\"instance_second_state\":");
  PrintMethodCacheStateProtocolCategory(instance_second_state);
  std::printf(",\"class_self_state\":");
  PrintMethodCacheStateProtocolCategory(class_self_state);
  std::printf(",\"known_class_state\":");
  PrintMethodCacheStateProtocolCategory(known_class_state);
  std::printf(",\"fallback_first_state\":");
  PrintMethodCacheStateProtocolCategory(fallback_first_state);
  std::printf(",\"fallback_second_state\":");
  PrintMethodCacheStateProtocolCategory(fallback_second_state);
  std::printf(",\"instance_entry\":");
  PrintMethodCacheEntryWithProbeCounts(instance_entry);
  std::printf(",\"class_entry\":");
  PrintMethodCacheEntryWithProbeCounts(class_entry);
  std::printf(",\"fallback_entry\":");
  PrintMethodCacheEntryWithProbeCounts(fallback_entry);
  std::printf("}\n");
  return 0;
}
