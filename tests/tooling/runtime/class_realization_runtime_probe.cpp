#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdio>
#include <string>

namespace {

constexpr long long kDispatchModulus = 2147483629LL;

void PrintJsonStringOrNull(const char *value) {
  if (value == nullptr) {
    std::printf("null");
    return;
  }
  std::printf("\"");
  for (const unsigned char *cursor =
           reinterpret_cast<const unsigned char *>(value);
       *cursor != 0U; ++cursor) {
    switch (*cursor) {
      case '\\':
        std::printf("\\\\");
        break;
      case '"':
        std::printf("\\\"");
        break;
      case '\n':
        std::printf("\\n");
        break;
      case '\r':
        std::printf("\\r");
        break;
      case '\t':
        std::printf("\\t");
        break;
      default:
        std::printf("%c", static_cast<char>(*cursor));
        break;
    }
  }
  std::printf("\"");
}

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

void StabilizeNullableCString(const char *source, std::string &storage,
                              const char *&field) {
  storage = source != nullptr ? source : "";
  field = storage.empty() ? nullptr : storage.c_str();
}

void StabilizeRegistrationState(
    objc3_runtime_registration_state_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage,
    std::string &rejected_module_storage,
    std::string &rejected_identity_storage) {
  StabilizeNullableCString(snapshot.last_registered_module_name, module_storage,
                           snapshot.last_registered_module_name);
  StabilizeNullableCString(snapshot.last_registered_translation_unit_identity_key,
                           identity_storage,
                           snapshot.last_registered_translation_unit_identity_key);
  StabilizeNullableCString(snapshot.last_rejected_module_name,
                           rejected_module_storage,
                           snapshot.last_rejected_module_name);
  StabilizeNullableCString(snapshot.last_rejected_translation_unit_identity_key,
                           rejected_identity_storage,
                           snapshot.last_rejected_translation_unit_identity_key);
}

void StabilizeSelectorTableState(
    objc3_runtime_selector_lookup_table_state_snapshot &snapshot,
    std::string &selector_storage) {
  StabilizeNullableCString(snapshot.last_materialized_selector, selector_storage,
                           snapshot.last_materialized_selector);
}

void StabilizeMethodCacheState(
    objc3_runtime_method_cache_state_snapshot &snapshot,
    std::string &selector_storage, std::string &class_storage,
    std::string &owner_storage) {
  StabilizeNullableCString(snapshot.last_selector, selector_storage,
                           snapshot.last_selector);
  StabilizeNullableCString(snapshot.last_resolved_class_name, class_storage,
                           snapshot.last_resolved_class_name);
  StabilizeNullableCString(snapshot.last_resolved_owner_identity, owner_storage,
                           snapshot.last_resolved_owner_identity);
}

void StabilizeMethodCacheEntry(
    objc3_runtime_method_cache_entry_snapshot &snapshot,
    std::string &selector_storage, std::string &class_storage,
    std::string &owner_storage) {
  StabilizeNullableCString(snapshot.selector, selector_storage, snapshot.selector);
  StabilizeNullableCString(snapshot.resolved_class_name, class_storage,
                           snapshot.resolved_class_name);
  StabilizeNullableCString(snapshot.resolved_owner_identity, owner_storage,
                           snapshot.resolved_owner_identity);
}

void PrintRegistrationState(
    const objc3_runtime_registration_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"registered_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_image_count));
  std::printf("\"registered_descriptor_total\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_descriptor_total));
  std::printf("\"next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.next_expected_registration_order_ordinal));
  std::printf("\"last_successful_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_successful_registration_order_ordinal));
  std::printf("\"last_registration_status\":%d,",
              snapshot.last_registration_status);
  std::printf("\"last_registered_module_name\":");
  PrintJsonStringOrNull(snapshot.last_registered_module_name);
  std::printf(",\"last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(snapshot.last_registered_translation_unit_identity_key);
  std::printf(",\"last_rejected_module_name\":");
  PrintJsonStringOrNull(snapshot.last_rejected_module_name);
  std::printf(",\"last_rejected_translation_unit_identity_key\":");
  PrintJsonStringOrNull(snapshot.last_rejected_translation_unit_identity_key);
  std::printf(",\"last_rejected_registration_order_ordinal\":%llu",
              static_cast<unsigned long long>(
                  snapshot.last_rejected_registration_order_ordinal));
  std::printf("}");
}

void PrintSelectorTableState(
    const objc3_runtime_selector_lookup_table_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"selector_table_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.selector_table_entry_count));
  std::printf("\"metadata_backed_selector_count\":%llu,",
              static_cast<unsigned long long>(snapshot.metadata_backed_selector_count));
  std::printf("\"dynamic_selector_count\":%llu,",
              static_cast<unsigned long long>(snapshot.dynamic_selector_count));
  std::printf("\"metadata_provider_edge_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.metadata_provider_edge_count));
  std::printf("\"last_materialized_selector\":");
  PrintJsonStringOrNull(snapshot.last_materialized_selector);
  std::printf(",\"last_materialized_stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.last_materialized_stable_id));
  std::printf("\"last_materialized_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_materialized_registration_order_ordinal));
  std::printf("\"last_materialized_selector_pool_index\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_materialized_selector_pool_index));
  std::printf("\"last_materialized_from_metadata\":%d",
              snapshot.last_materialized_from_metadata);
  std::printf("}");
}

void PrintMethodCacheState(
    const objc3_runtime_method_cache_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"cache_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_entry_count));
  std::printf("\"cache_hit_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_hit_count));
  std::printf("\"cache_miss_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_miss_count));
  std::printf("\"slow_path_lookup_count\":%llu,",
              static_cast<unsigned long long>(snapshot.slow_path_lookup_count));
  std::printf("\"live_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_dispatch_count));
  std::printf("\"fallback_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.fallback_dispatch_count));
  std::printf("\"last_selector_stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.last_selector_stable_id));
  std::printf("\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf("\"last_category_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.last_category_probe_count));
  std::printf("\"last_protocol_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.last_protocol_probe_count));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_fell_back\":%d,",
              snapshot.last_dispatch_fell_back);
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(",\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

void PrintMethodCacheEntry(
    const objc3_runtime_method_cache_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"resolved\":%d,", snapshot.resolved);
  std::printf("\"dispatch_family_is_class\":%d,",
              snapshot.dispatch_family_is_class);
  std::printf("\"normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.normalized_receiver_identity));
  std::printf("\"selector_stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.selector_stable_id));
  std::printf("\"parameter_count\":%llu,",
              static_cast<unsigned long long>(snapshot.parameter_count));
  std::printf("\"category_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.category_probe_count));
  std::printf("\"protocol_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.protocol_probe_count));
  std::printf("\"selector\":");
  PrintJsonStringOrNull(snapshot.selector);
  std::printf(",\"resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.resolved_class_name);
  std::printf(",\"resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.resolved_owner_identity);
  std::printf("}");
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
  PrintRegistrationState(registration_state);
  std::printf(",\"selector_table_state\":");
  PrintSelectorTableState(selector_table_state);
  std::printf(",\"inherited_state\":");
  PrintMethodCacheState(inherited_state);
  std::printf(",\"category_state\":");
  PrintMethodCacheState(category_state);
  std::printf(",\"class_state\":");
  PrintMethodCacheState(class_state);
  std::printf(",\"known_class_state\":");
  PrintMethodCacheState(known_class_state);
  std::printf(",\"protocol_fallback_state\":");
  PrintMethodCacheState(protocol_fallback_state);
  std::printf(",\"protocol_fallback_cached_state\":");
  PrintMethodCacheState(protocol_fallback_cached_state);
  std::printf(",\"inherited_entry\":");
  PrintMethodCacheEntry(inherited_entry);
  std::printf(",\"category_entry\":");
  PrintMethodCacheEntry(category_entry);
  std::printf(",\"known_class_entry\":");
  PrintMethodCacheEntry(known_class_entry);
  std::printf(",\"protocol_fallback_entry\":");
  PrintMethodCacheEntry(protocol_fallback_entry);
  std::printf("}\n");
  return 0;
}
