#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdio>
#include <string>

namespace {

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

void StabilizeNullableCString(const char *source, std::string &storage,
                              const char *&field) {
  storage = source != nullptr ? source : "";
  field = storage.empty() ? nullptr : storage.c_str();
}

void StabilizeRegistrationState(
    objc3_runtime_registration_state_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage) {
  StabilizeNullableCString(snapshot.last_registered_module_name, module_storage,
                           snapshot.last_registered_module_name);
  StabilizeNullableCString(snapshot.last_registered_translation_unit_identity_key,
                           identity_storage,
                           snapshot.last_registered_translation_unit_identity_key);
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
  std::printf("\"last_registration_status\":%d,", snapshot.last_registration_status);
  std::printf("\"last_registered_module_name\":");
  PrintJsonStringOrNull(snapshot.last_registered_module_name);
  std::printf(",\"last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(snapshot.last_registered_translation_unit_identity_key);
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
  std::printf("\"last_materialized_selector\":");
  PrintJsonStringOrNull(snapshot.last_materialized_selector);
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
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(",\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_fell_back\":%d,",
              snapshot.last_dispatch_fell_back);
  std::printf("\"last_resolved_class_name\":");
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
  objc3_runtime_method_cache_state_snapshot instance_first_state{};
  objc3_runtime_method_cache_state_snapshot instance_second_state{};
  objc3_runtime_method_cache_state_snapshot class_state{};
  objc3_runtime_method_cache_state_snapshot known_class_state{};
  objc3_runtime_method_cache_state_snapshot category_state{};
  objc3_runtime_method_cache_entry_snapshot instance_entry{};
  objc3_runtime_method_cache_entry_snapshot class_entry{};
  objc3_runtime_method_cache_entry_snapshot category_entry{};

  std::string registration_module_storage;
  std::string registration_identity_storage;
  std::string selector_table_last_storage;
  std::string instance_first_selector_storage;
  std::string instance_first_class_storage;
  std::string instance_first_owner_storage;
  std::string instance_second_selector_storage;
  std::string instance_second_class_storage;
  std::string instance_second_owner_storage;
  std::string class_selector_storage;
  std::string class_class_storage;
  std::string class_owner_storage;
  std::string known_class_selector_storage;
  std::string known_class_class_storage;
  std::string known_class_owner_storage;
  std::string category_selector_storage;
  std::string category_class_storage;
  std::string category_owner_storage;
  std::string instance_entry_selector_storage;
  std::string instance_entry_class_storage;
  std::string instance_entry_owner_storage;
  std::string class_entry_selector_storage;
  std::string class_entry_class_storage;
  std::string class_entry_owner_storage;
  std::string category_entry_selector_storage;
  std::string category_entry_class_storage;
  std::string category_entry_owner_storage;

  (void)objc3_runtime_copy_registration_state_for_testing(&registration_state);
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &selector_table_state);
  StabilizeRegistrationState(registration_state, registration_module_storage,
                             registration_identity_storage);
  StabilizeSelectorTableState(selector_table_state, selector_table_last_storage);

  const int instance_first =
      objc3_runtime_dispatch_i32(1025, "value:extra:", 7, 8, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&instance_first_state);
  StabilizeMethodCacheState(instance_first_state, instance_first_selector_storage,
                            instance_first_class_storage,
                            instance_first_owner_storage);

  const int instance_second =
      objc3_runtime_dispatch_i32(1025, "value:extra:", 7, 8, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&instance_second_state);
  StabilizeMethodCacheState(instance_second_state,
                            instance_second_selector_storage,
                            instance_second_class_storage,
                            instance_second_owner_storage);

  const int class_value =
      objc3_runtime_dispatch_i32(1026, "classValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&class_state);
  StabilizeMethodCacheState(class_state, class_selector_storage,
                            class_class_storage, class_owner_storage);

  const int known_class_value =
      objc3_runtime_dispatch_i32(1024, "classValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&known_class_state);
  StabilizeMethodCacheState(known_class_state, known_class_selector_storage,
                            known_class_class_storage,
                            known_class_owner_storage);

  const int category_value =
      objc3_runtime_dispatch_i32(1025, "tracedValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&category_state);
  StabilizeMethodCacheState(category_state, category_selector_storage,
                            category_class_storage, category_owner_storage);

  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      1025, "value:extra:", &instance_entry);
  StabilizeMethodCacheEntry(instance_entry, instance_entry_selector_storage,
                            instance_entry_class_storage,
                            instance_entry_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      1024, "classValue", &class_entry);
  StabilizeMethodCacheEntry(class_entry, class_entry_selector_storage,
                            class_entry_class_storage,
                            class_entry_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      1025, "tracedValue", &category_entry);
  StabilizeMethodCacheEntry(category_entry, category_entry_selector_storage,
                            category_entry_class_storage,
                            category_entry_owner_storage);

  std::printf("{");
  std::printf("\"instance_first\":%d,", instance_first);
  std::printf("\"instance_second\":%d,", instance_second);
  std::printf("\"class_value\":%d,", class_value);
  std::printf("\"known_class_value\":%d,", known_class_value);
  std::printf("\"category_value\":%d,", category_value);
  std::printf("\"registration_state\":");
  PrintRegistrationState(registration_state);
  std::printf(",\"selector_table_state\":");
  PrintSelectorTableState(selector_table_state);
  std::printf(",\"instance_first_state\":");
  PrintMethodCacheState(instance_first_state);
  std::printf(",\"instance_second_state\":");
  PrintMethodCacheState(instance_second_state);
  std::printf(",\"class_state\":");
  PrintMethodCacheState(class_state);
  std::printf(",\"known_class_state\":");
  PrintMethodCacheState(known_class_state);
  std::printf(",\"category_state\":");
  PrintMethodCacheState(category_state);
  std::printf(",\"instance_entry\":");
  PrintMethodCacheEntry(instance_entry);
  std::printf(",\"class_entry\":");
  PrintMethodCacheEntry(class_entry);
  std::printf(",\"category_entry\":");
  PrintMethodCacheEntry(category_entry);
  std::printf("}");
  return 0;
}
