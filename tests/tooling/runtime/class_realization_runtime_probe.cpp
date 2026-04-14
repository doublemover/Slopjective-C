#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_stabilizers.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>
#include <string>

namespace {

using objc3c::runtime::probe::PrintMethodCacheEntryWithProbeCounts;
using objc3c::runtime::probe::PrintMethodCacheStateFull;
using objc3c::runtime::probe::PrintRegistrationStateFull;
using objc3c::runtime::probe::PrintSelectorTableStateFull;

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
  if (receiver == 0) {
    return 0;
  }
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
  objc3_runtime_method_cache_state_snapshot inherited_state{};
  objc3_runtime_method_cache_state_snapshot category_state{};
  objc3_runtime_method_cache_state_snapshot class_state{};
  objc3_runtime_method_cache_state_snapshot known_class_state{};
  objc3_runtime_method_cache_state_snapshot protocol_fallback_state{};
  objc3_runtime_method_cache_state_snapshot protocol_fallback_cached_state{};
  objc3_runtime_method_cache_entry_snapshot inherited_entry{};
  objc3_runtime_method_cache_entry_snapshot category_entry{};
  objc3_runtime_method_cache_entry_snapshot known_class_entry{};
  objc3_runtime_method_cache_entry_snapshot protocol_fallback_entry{};

  std::string registration_module_storage;
  std::string registration_identity_storage;
  std::string registration_rejected_module_storage;
  std::string registration_rejected_identity_storage;
  std::string selector_table_last_storage;
  std::string inherited_selector_storage;
  std::string inherited_class_storage;
  std::string inherited_owner_storage;
  std::string category_selector_storage;
  std::string category_class_storage;
  std::string category_owner_storage;
  std::string class_selector_storage;
  std::string class_class_storage;
  std::string class_owner_storage;
  std::string known_class_selector_storage;
  std::string known_class_class_storage;
  std::string known_class_owner_storage;
  std::string protocol_fallback_selector_storage;
  std::string protocol_fallback_class_storage;
  std::string protocol_fallback_owner_storage;
  std::string protocol_fallback_cached_selector_storage;
  std::string protocol_fallback_cached_class_storage;
  std::string protocol_fallback_cached_owner_storage;
  std::string inherited_entry_selector_storage;
  std::string inherited_entry_class_storage;
  std::string inherited_entry_owner_storage;
  std::string category_entry_selector_storage;
  std::string category_entry_class_storage;
  std::string category_entry_owner_storage;
  std::string known_class_entry_selector_storage;
  std::string known_class_entry_class_storage;
  std::string known_class_entry_owner_storage;
  std::string protocol_fallback_entry_selector_storage;
  std::string protocol_fallback_entry_class_storage;
  std::string protocol_fallback_entry_owner_storage;

  const int inherited_value =
      objc3_runtime_dispatch_i32(1042, "inheritedValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&inherited_state);
  StabilizeMethodCacheState(inherited_state, inherited_selector_storage,
                            inherited_class_storage, inherited_owner_storage);

  const int category_value =
      objc3_runtime_dispatch_i32(1042, "tracedValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&category_state);
  StabilizeMethodCacheState(category_state, category_selector_storage,
                            category_class_storage, category_owner_storage);

  const int class_value =
      objc3_runtime_dispatch_i32(1043, "classValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&class_state);
  StabilizeMethodCacheState(class_state, class_selector_storage,
                            class_class_storage, class_owner_storage);

  const int known_class_value =
      objc3_runtime_dispatch_i32(1041, "classValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&known_class_state);
  StabilizeMethodCacheState(known_class_state, known_class_selector_storage,
                            known_class_class_storage,
                            known_class_owner_storage);

  const int protocol_fallback =
      objc3_runtime_dispatch_i32(1042, "ignoredValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &protocol_fallback_state);
  StabilizeMethodCacheState(protocol_fallback_state,
                            protocol_fallback_selector_storage,
                            protocol_fallback_class_storage,
                            protocol_fallback_owner_storage);

  const int protocol_fallback_cached =
      objc3_runtime_dispatch_i32(1042, "ignoredValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &protocol_fallback_cached_state);
  StabilizeMethodCacheState(protocol_fallback_cached_state,
                            protocol_fallback_cached_selector_storage,
                            protocol_fallback_cached_class_storage,
                            protocol_fallback_cached_owner_storage);

  const int protocol_fallback_expected =
      ComputeFallbackDispatch(1042, "ignoredValue", 0, 0, 0, 0);

  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      1042, "inheritedValue", &inherited_entry);
  StabilizeMethodCacheEntry(inherited_entry, inherited_entry_selector_storage,
                            inherited_entry_class_storage,
                            inherited_entry_owner_storage);

  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      1042, "tracedValue", &category_entry);
  StabilizeMethodCacheEntry(category_entry, category_entry_selector_storage,
                            category_entry_class_storage,
                            category_entry_owner_storage);

  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      1041, "classValue", &known_class_entry);
  StabilizeMethodCacheEntry(known_class_entry,
                            known_class_entry_selector_storage,
                            known_class_entry_class_storage,
                            known_class_entry_owner_storage);

  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      1042, "ignoredValue", &protocol_fallback_entry);
  StabilizeMethodCacheEntry(protocol_fallback_entry,
                            protocol_fallback_entry_selector_storage,
                            protocol_fallback_entry_class_storage,
                            protocol_fallback_entry_owner_storage);

  (void)objc3_runtime_copy_registration_state_for_testing(&registration_state);
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &selector_table_state);
  StabilizeRegistrationState(
      registration_state, registration_module_storage,
      registration_identity_storage, registration_rejected_module_storage,
      registration_rejected_identity_storage);
  StabilizeSelectorTableState(selector_table_state, selector_table_last_storage);

  std::printf("{");
  std::printf("\"inherited_value\":%d,", inherited_value);
  std::printf("\"category_value\":%d,", category_value);
  std::printf("\"class_value\":%d,", class_value);
  std::printf("\"known_class_value\":%d,", known_class_value);
  std::printf("\"protocol_fallback\":%d,", protocol_fallback);
  std::printf("\"protocol_fallback_cached\":%d,",
              protocol_fallback_cached);
  std::printf("\"protocol_fallback_expected\":%d,",
              protocol_fallback_expected);
  std::printf("\"registration_state\":");
  PrintRegistrationStateFull(registration_state);
  std::printf(",\"selector_table_state\":");
  PrintSelectorTableStateFull(selector_table_state);
  std::printf(",\"inherited_state\":");
  PrintMethodCacheStateFull(inherited_state);
  std::printf(",\"category_state\":");
  PrintMethodCacheStateFull(category_state);
  std::printf(",\"class_state\":");
  PrintMethodCacheStateFull(class_state);
  std::printf(",\"known_class_state\":");
  PrintMethodCacheStateFull(known_class_state);
  std::printf(",\"protocol_fallback_state\":");
  PrintMethodCacheStateFull(protocol_fallback_state);
  std::printf(",\"protocol_fallback_cached_state\":");
  PrintMethodCacheStateFull(protocol_fallback_cached_state);
  std::printf(",\"inherited_entry\":");
  PrintMethodCacheEntryWithProbeCounts(inherited_entry);
  std::printf(",\"category_entry\":");
  PrintMethodCacheEntryWithProbeCounts(category_entry);
  std::printf(",\"known_class_entry\":");
  PrintMethodCacheEntryWithProbeCounts(known_class_entry);
  std::printf(",\"protocol_fallback_entry\":");
  PrintMethodCacheEntryWithProbeCounts(protocol_fallback_entry);
  std::printf("}\n");
  return 0;
}
