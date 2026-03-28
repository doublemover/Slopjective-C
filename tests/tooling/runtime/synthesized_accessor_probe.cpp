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
  objc3_runtime_method_cache_entry_snapshot count_entry{};
  objc3_runtime_method_cache_entry_snapshot set_count_entry{};
  objc3_runtime_method_cache_entry_snapshot enabled_entry{};
  objc3_runtime_method_cache_entry_snapshot set_enabled_entry{};
  objc3_runtime_method_cache_entry_snapshot value_entry{};
  objc3_runtime_method_cache_entry_snapshot set_value_entry{};

  std::string registration_module_storage;
  std::string registration_identity_storage;
  std::string selector_table_last_storage;
  std::string count_selector_storage;
  std::string count_class_storage;
  std::string count_owner_storage;
  std::string set_count_selector_storage;
  std::string set_count_class_storage;
  std::string set_count_owner_storage;
  std::string enabled_selector_storage;
  std::string enabled_class_storage;
  std::string enabled_owner_storage;
  std::string set_enabled_selector_storage;
  std::string set_enabled_class_storage;
  std::string set_enabled_owner_storage;
  std::string value_selector_storage;
  std::string value_class_storage;
  std::string value_owner_storage;
  std::string set_value_selector_storage;
  std::string set_value_class_storage;
  std::string set_value_owner_storage;

  (void)objc3_runtime_copy_registration_state_for_testing(&registration_state);
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &selector_table_state);
  StabilizeRegistrationState(registration_state, registration_module_storage,
                             registration_identity_storage);
  StabilizeSelectorTableState(selector_table_state, selector_table_last_storage);

  const int widget_instance =
      objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  const int set_count_result =
      objc3_runtime_dispatch_i32(widget_instance, "setCount:", 37, 0, 0, 0);
  const int count_value =
      objc3_runtime_dispatch_i32(widget_instance, "count", 0, 0, 0, 0);
  const int set_enabled_result =
      objc3_runtime_dispatch_i32(widget_instance, "setEnabled:", 1, 0, 0, 0);
  const int enabled_value =
      objc3_runtime_dispatch_i32(widget_instance, "enabled", 0, 0, 0, 0);
  const int set_value_result =
      objc3_runtime_dispatch_i32(widget_instance, "setValue:", 55, 0, 0, 0);
  const int value_result =
      objc3_runtime_dispatch_i32(widget_instance, "value", 0, 0, 0, 0);

  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      widget_instance, "count", &count_entry);
  StabilizeMethodCacheEntry(count_entry, count_selector_storage,
                            count_class_storage, count_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      widget_instance, "setCount:", &set_count_entry);
  StabilizeMethodCacheEntry(set_count_entry, set_count_selector_storage,
                            set_count_class_storage, set_count_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      widget_instance, "enabled", &enabled_entry);
  StabilizeMethodCacheEntry(enabled_entry, enabled_selector_storage,
                            enabled_class_storage, enabled_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      widget_instance, "setEnabled:", &set_enabled_entry);
  StabilizeMethodCacheEntry(set_enabled_entry, set_enabled_selector_storage,
                            set_enabled_class_storage,
                            set_enabled_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      widget_instance, "value", &value_entry);
  StabilizeMethodCacheEntry(value_entry, value_selector_storage,
                            value_class_storage, value_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      widget_instance, "setValue:", &set_value_entry);
  StabilizeMethodCacheEntry(set_value_entry, set_value_selector_storage,
                            set_value_class_storage, set_value_owner_storage);

  std::printf("{");
  std::printf("\"widget_instance\":%d,", widget_instance);
  std::printf("\"set_count_result\":%d,", set_count_result);
  std::printf("\"count_value\":%d,", count_value);
  std::printf("\"set_enabled_result\":%d,", set_enabled_result);
  std::printf("\"enabled_value\":%d,", enabled_value);
  std::printf("\"set_value_result\":%d,", set_value_result);
  std::printf("\"value_result\":%d,", value_result);
  std::printf("\"registration_state\":");
  PrintRegistrationState(registration_state);
  std::printf(",\"selector_table_state\":");
  PrintSelectorTableState(selector_table_state);
  std::printf(",\"count_entry\":");
  PrintMethodCacheEntry(count_entry);
  std::printf(",\"set_count_entry\":");
  PrintMethodCacheEntry(set_count_entry);
  std::printf(",\"enabled_entry\":");
  PrintMethodCacheEntry(enabled_entry);
  std::printf(",\"set_enabled_entry\":");
  PrintMethodCacheEntry(set_enabled_entry);
  std::printf(",\"value_entry\":");
  PrintMethodCacheEntry(value_entry);
  std::printf(",\"set_value_entry\":");
  PrintMethodCacheEntry(set_value_entry);
  std::printf("}");
  return 0;
}
