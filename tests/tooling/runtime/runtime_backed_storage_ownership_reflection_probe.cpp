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

struct StablePropertyEntry {
  objc3_runtime_property_entry_snapshot snapshot{};
  std::string queried_class_name;
  std::string resolved_class_name;
  std::string property_name;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string getter_selector;
  std::string setter_selector;
  std::string effective_getter_selector;
  std::string effective_setter_selector;
  std::string ivar_binding_symbol;
  std::string synthesized_binding_symbol;
  std::string ivar_layout_symbol;
  std::string property_attribute_profile;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  std::string accessor_ownership_profile;
  std::string getter_owner_identity;
  std::string setter_owner_identity;
};

struct StableRealizedClassEntry {
  objc3_runtime_realized_class_entry_snapshot snapshot{};
  std::string module_name;
  std::string translation_unit_identity_key;
  std::string class_name;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
  std::string super_class_owner_identity;
  std::string super_metaclass_owner_identity;
  std::string attached_category_owner_identity;
  std::string attached_category_name;
};

void StabilizePropertyEntry(StablePropertyEntry &entry) {
  StabilizeNullableCString(entry.snapshot.queried_class_name,
                           entry.queried_class_name,
                           entry.snapshot.queried_class_name);
  StabilizeNullableCString(entry.snapshot.resolved_class_name,
                           entry.resolved_class_name,
                           entry.snapshot.resolved_class_name);
  StabilizeNullableCString(entry.snapshot.property_name, entry.property_name,
                           entry.snapshot.property_name);
  StabilizeNullableCString(entry.snapshot.declaration_owner_identity,
                           entry.declaration_owner_identity,
                           entry.snapshot.declaration_owner_identity);
  StabilizeNullableCString(entry.snapshot.export_owner_identity,
                           entry.export_owner_identity,
                           entry.snapshot.export_owner_identity);
  StabilizeNullableCString(entry.snapshot.getter_selector,
                           entry.getter_selector,
                           entry.snapshot.getter_selector);
  StabilizeNullableCString(entry.snapshot.setter_selector,
                           entry.setter_selector,
                           entry.snapshot.setter_selector);
  StabilizeNullableCString(entry.snapshot.effective_getter_selector,
                           entry.effective_getter_selector,
                           entry.snapshot.effective_getter_selector);
  StabilizeNullableCString(entry.snapshot.effective_setter_selector,
                           entry.effective_setter_selector,
                           entry.snapshot.effective_setter_selector);
  StabilizeNullableCString(entry.snapshot.ivar_binding_symbol,
                           entry.ivar_binding_symbol,
                           entry.snapshot.ivar_binding_symbol);
  StabilizeNullableCString(entry.snapshot.synthesized_binding_symbol,
                           entry.synthesized_binding_symbol,
                           entry.snapshot.synthesized_binding_symbol);
  StabilizeNullableCString(entry.snapshot.ivar_layout_symbol,
                           entry.ivar_layout_symbol,
                           entry.snapshot.ivar_layout_symbol);
  StabilizeNullableCString(entry.snapshot.property_attribute_profile,
                           entry.property_attribute_profile,
                           entry.snapshot.property_attribute_profile);
  StabilizeNullableCString(entry.snapshot.ownership_lifetime_profile,
                           entry.ownership_lifetime_profile,
                           entry.snapshot.ownership_lifetime_profile);
  StabilizeNullableCString(entry.snapshot.ownership_runtime_hook_profile,
                           entry.ownership_runtime_hook_profile,
                           entry.snapshot.ownership_runtime_hook_profile);
  StabilizeNullableCString(entry.snapshot.accessor_ownership_profile,
                           entry.accessor_ownership_profile,
                           entry.snapshot.accessor_ownership_profile);
  StabilizeNullableCString(entry.snapshot.getter_owner_identity,
                           entry.getter_owner_identity,
                           entry.snapshot.getter_owner_identity);
  StabilizeNullableCString(entry.snapshot.setter_owner_identity,
                           entry.setter_owner_identity,
                           entry.snapshot.setter_owner_identity);
}

void StabilizeRealizedClassEntry(StableRealizedClassEntry &entry) {
  StabilizeNullableCString(entry.snapshot.module_name, entry.module_name,
                           entry.snapshot.module_name);
  StabilizeNullableCString(entry.snapshot.translation_unit_identity_key,
                           entry.translation_unit_identity_key,
                           entry.snapshot.translation_unit_identity_key);
  StabilizeNullableCString(entry.snapshot.class_name, entry.class_name,
                           entry.snapshot.class_name);
  StabilizeNullableCString(entry.snapshot.class_owner_identity,
                           entry.class_owner_identity,
                           entry.snapshot.class_owner_identity);
  StabilizeNullableCString(entry.snapshot.metaclass_owner_identity,
                           entry.metaclass_owner_identity,
                           entry.snapshot.metaclass_owner_identity);
  StabilizeNullableCString(entry.snapshot.super_class_owner_identity,
                           entry.super_class_owner_identity,
                           entry.snapshot.super_class_owner_identity);
  StabilizeNullableCString(entry.snapshot.super_metaclass_owner_identity,
                           entry.super_metaclass_owner_identity,
                           entry.snapshot.super_metaclass_owner_identity);
  StabilizeNullableCString(entry.snapshot.last_attached_category_owner_identity,
                           entry.attached_category_owner_identity,
                           entry.snapshot.last_attached_category_owner_identity);
  StabilizeNullableCString(entry.snapshot.last_attached_category_name,
                           entry.attached_category_name,
                           entry.snapshot.last_attached_category_name);
}

void PrintPropertyEntry(const objc3_runtime_property_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"setter_available\":%d,", snapshot.setter_available);
  std::printf("\"has_runtime_getter\":%d,", snapshot.has_runtime_getter);
  std::printf("\"has_runtime_setter\":%d,", snapshot.has_runtime_setter);
  std::printf("\"base_identity\":%llu,",
              static_cast<unsigned long long>(snapshot.base_identity));
  std::printf("\"slot_index\":%llu,",
              static_cast<unsigned long long>(snapshot.slot_index));
  std::printf("\"offset_bytes\":%llu,",
              static_cast<unsigned long long>(snapshot.offset_bytes));
  std::printf("\"size_bytes\":%llu,",
              static_cast<unsigned long long>(snapshot.size_bytes));
  std::printf("\"alignment_bytes\":%llu,",
              static_cast<unsigned long long>(snapshot.alignment_bytes));
  std::printf("\"instance_size_bytes\":%llu,",
              static_cast<unsigned long long>(snapshot.instance_size_bytes));
  std::printf("\"queried_class_name\":");
  PrintJsonStringOrNull(snapshot.queried_class_name);
  std::printf(",\"resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.resolved_class_name);
  std::printf(",\"property_name\":");
  PrintJsonStringOrNull(snapshot.property_name);
  std::printf(",\"effective_getter_selector\":");
  PrintJsonStringOrNull(snapshot.effective_getter_selector);
  std::printf(",\"effective_setter_selector\":");
  PrintJsonStringOrNull(snapshot.effective_setter_selector);
  std::printf(",\"property_attribute_profile\":");
  PrintJsonStringOrNull(snapshot.property_attribute_profile);
  std::printf(",\"ownership_lifetime_profile\":");
  PrintJsonStringOrNull(snapshot.ownership_lifetime_profile);
  std::printf(",\"ownership_runtime_hook_profile\":");
  PrintJsonStringOrNull(snapshot.ownership_runtime_hook_profile);
  std::printf(",\"accessor_ownership_profile\":");
  PrintJsonStringOrNull(snapshot.accessor_ownership_profile);
  std::printf(",\"getter_owner_identity\":");
  PrintJsonStringOrNull(snapshot.getter_owner_identity);
  std::printf(",\"setter_owner_identity\":");
  PrintJsonStringOrNull(snapshot.setter_owner_identity);
  std::printf("}");
}

void PrintRealizedClassEntry(
    const objc3_runtime_realized_class_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"base_identity\":%llu,",
              static_cast<unsigned long long>(snapshot.base_identity));
  std::printf("\"runtime_property_accessor_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.runtime_property_accessor_count));
  std::printf("\"runtime_instance_size_bytes\":%llu,",
              static_cast<unsigned long long>(snapshot.runtime_instance_size_bytes));
  std::printf("\"class_name\":");
  PrintJsonStringOrNull(snapshot.class_name);
  std::printf(",\"class_owner_identity\":");
  PrintJsonStringOrNull(snapshot.class_owner_identity);
  std::printf("}");
}

void PrintStorageAccessorImplementation(
    const objc3_runtime_storage_accessor_implementation_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"property_registry_ready\":%llu,",
              static_cast<unsigned long long>(snapshot.property_registry_ready));
  std::printf("\"runtime_accessor_dispatch_ready\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.runtime_accessor_dispatch_ready));
  std::printf("\"runtime_layout_ready\":%llu,",
              static_cast<unsigned long long>(snapshot.runtime_layout_ready));
  std::printf("\"reflection_query_ready\":%llu,",
              static_cast<unsigned long long>(snapshot.reflection_query_ready));
  std::printf("\"deterministic\":%llu,",
              static_cast<unsigned long long>(snapshot.deterministic));
  std::printf("\"property_registry_state_snapshot_symbol\":");
  PrintJsonStringOrNull(snapshot.property_registry_state_snapshot_symbol);
  std::printf(",\"property_entry_snapshot_symbol\":");
  PrintJsonStringOrNull(snapshot.property_entry_snapshot_symbol);
  std::printf(",\"current_property_read_symbol\":");
  PrintJsonStringOrNull(snapshot.current_property_read_symbol);
  std::printf(",\"current_property_write_symbol\":");
  PrintJsonStringOrNull(snapshot.current_property_write_symbol);
  std::printf(",\"current_property_exchange_symbol\":");
  PrintJsonStringOrNull(snapshot.current_property_exchange_symbol);
  std::printf(",\"bind_current_property_context_symbol\":");
  PrintJsonStringOrNull(snapshot.bind_current_property_context_symbol);
  std::printf(",\"clear_current_property_context_symbol\":");
  PrintJsonStringOrNull(snapshot.clear_current_property_context_symbol);
  std::printf(",\"weak_current_property_load_symbol\":");
  PrintJsonStringOrNull(snapshot.weak_current_property_load_symbol);
  std::printf(",\"weak_current_property_store_symbol\":");
  PrintJsonStringOrNull(snapshot.weak_current_property_store_symbol);
  std::printf(",\"implementation_model\":");
  PrintJsonStringOrNull(snapshot.implementation_model);
  std::printf(",\"reflection_model\":");
  PrintJsonStringOrNull(snapshot.reflection_model);
  std::printf(",\"fail_closed_model\":");
  PrintJsonStringOrNull(snapshot.fail_closed_model);
  std::printf("}");
}

void LoadProperty(const char *class_name, const char *property_name,
                  StablePropertyEntry &entry) {
  (void)objc3_runtime_copy_property_entry_for_testing(class_name, property_name,
                                                      &entry.snapshot);
  StabilizePropertyEntry(entry);
}

}  // namespace

int main() {
  StableRealizedClassEntry box_entry;
  StablePropertyEntry current_value;
  StablePropertyEntry copied_value;
  StablePropertyEntry weak_value;
  StablePropertyEntry borrowed_value;
  StablePropertyEntry guarded_value;
  objc3_runtime_storage_accessor_implementation_snapshot implementation{};

  (void)objc3_runtime_copy_realized_class_entry_for_testing("Box",
                                                            &box_entry.snapshot);
  StabilizeRealizedClassEntry(box_entry);
  LoadProperty("Box", "currentValue", current_value);
  LoadProperty("Box", "copiedValue", copied_value);
  LoadProperty("Box", "weakValue", weak_value);
  LoadProperty("Box", "borrowedValue", borrowed_value);
  LoadProperty("Box", "guardedValue", guarded_value);
  (void)objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing(
      &implementation);

  std::printf("{");
  std::printf("\"box_entry\":");
  PrintRealizedClassEntry(box_entry.snapshot);
  std::printf(",\"implementation_surface\":");
  PrintStorageAccessorImplementation(implementation);
  std::printf(",\"current_value_property\":");
  PrintPropertyEntry(current_value.snapshot);
  std::printf(",\"copied_value_property\":");
  PrintPropertyEntry(copied_value.snapshot);
  std::printf(",\"weak_value_property\":");
  PrintPropertyEntry(weak_value.snapshot);
  std::printf(",\"borrowed_value_property\":");
  PrintPropertyEntry(borrowed_value.snapshot);
  std::printf(",\"guarded_value_property\":");
  PrintPropertyEntry(guarded_value.snapshot);
  std::printf("}");
  return 0;
}
