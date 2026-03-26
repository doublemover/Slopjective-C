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

void StabilizePropertyRegistryState(
    objc3_runtime_property_registry_state_snapshot &snapshot,
    std::string &queried_class_storage, std::string &queried_property_storage,
    std::string &resolved_class_storage, std::string &resolved_owner_storage) {
  StabilizeNullableCString(snapshot.last_queried_class_name,
                           queried_class_storage,
                           snapshot.last_queried_class_name);
  StabilizeNullableCString(snapshot.last_queried_property_name,
                           queried_property_storage,
                           snapshot.last_queried_property_name);
  StabilizeNullableCString(snapshot.last_resolved_class_name,
                           resolved_class_storage,
                           snapshot.last_resolved_class_name);
  StabilizeNullableCString(snapshot.last_resolved_owner_identity,
                           resolved_owner_storage,
                           snapshot.last_resolved_owner_identity);
}

void StabilizePropertyEntry(
    objc3_runtime_property_entry_snapshot &snapshot,
    std::string &queried_class_storage, std::string &resolved_class_storage,
    std::string &property_name_storage,
    std::string &declaration_owner_storage,
    std::string &export_owner_storage, std::string &getter_selector_storage,
    std::string &setter_selector_storage,
    std::string &effective_getter_selector_storage,
    std::string &effective_setter_selector_storage,
    std::string &ivar_binding_storage,
    std::string &synthesized_binding_storage,
    std::string &layout_symbol_storage,
    std::string &getter_owner_storage,
    std::string &setter_owner_storage) {
  StabilizeNullableCString(snapshot.queried_class_name, queried_class_storage,
                           snapshot.queried_class_name);
  StabilizeNullableCString(snapshot.resolved_class_name, resolved_class_storage,
                           snapshot.resolved_class_name);
  StabilizeNullableCString(snapshot.property_name, property_name_storage,
                           snapshot.property_name);
  StabilizeNullableCString(snapshot.declaration_owner_identity,
                           declaration_owner_storage,
                           snapshot.declaration_owner_identity);
  StabilizeNullableCString(snapshot.export_owner_identity, export_owner_storage,
                           snapshot.export_owner_identity);
  StabilizeNullableCString(snapshot.getter_selector, getter_selector_storage,
                           snapshot.getter_selector);
  StabilizeNullableCString(snapshot.setter_selector, setter_selector_storage,
                           snapshot.setter_selector);
  StabilizeNullableCString(snapshot.effective_getter_selector,
                           effective_getter_selector_storage,
                           snapshot.effective_getter_selector);
  StabilizeNullableCString(snapshot.effective_setter_selector,
                           effective_setter_selector_storage,
                           snapshot.effective_setter_selector);
  StabilizeNullableCString(snapshot.ivar_binding_symbol, ivar_binding_storage,
                           snapshot.ivar_binding_symbol);
  StabilizeNullableCString(snapshot.synthesized_binding_symbol,
                           synthesized_binding_storage,
                           snapshot.synthesized_binding_symbol);
  StabilizeNullableCString(snapshot.ivar_layout_symbol, layout_symbol_storage,
                           snapshot.ivar_layout_symbol);
  StabilizeNullableCString(snapshot.getter_owner_identity,
                           getter_owner_storage,
                           snapshot.getter_owner_identity);
  StabilizeNullableCString(snapshot.setter_owner_identity,
                           setter_owner_storage,
                           snapshot.setter_owner_identity);
}

void StabilizeRealizedClassEntry(
    objc3_runtime_realized_class_entry_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage,
    std::string &class_storage, std::string &class_owner_storage,
    std::string &metaclass_owner_storage, std::string &super_class_storage,
    std::string &super_metaclass_storage, std::string &category_owner_storage,
    std::string &category_name_storage) {
  StabilizeNullableCString(snapshot.module_name, module_storage,
                           snapshot.module_name);
  StabilizeNullableCString(snapshot.translation_unit_identity_key,
                           identity_storage,
                           snapshot.translation_unit_identity_key);
  StabilizeNullableCString(snapshot.class_name, class_storage,
                           snapshot.class_name);
  StabilizeNullableCString(snapshot.class_owner_identity, class_owner_storage,
                           snapshot.class_owner_identity);
  StabilizeNullableCString(snapshot.metaclass_owner_identity,
                           metaclass_owner_storage,
                           snapshot.metaclass_owner_identity);
  StabilizeNullableCString(snapshot.super_class_owner_identity,
                           super_class_storage,
                           snapshot.super_class_owner_identity);
  StabilizeNullableCString(snapshot.super_metaclass_owner_identity,
                           super_metaclass_storage,
                           snapshot.super_metaclass_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_owner_identity,
                           category_owner_storage,
                           snapshot.last_attached_category_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_name,
                           category_name_storage,
                           snapshot.last_attached_category_name);
}

void PrintPropertyRegistryState(
    const objc3_runtime_property_registry_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"layout_ready_class_count\":%llu,",
              static_cast<unsigned long long>(snapshot.layout_ready_class_count));
  std::printf("\"reflectable_property_count\":%llu,",
              static_cast<unsigned long long>(snapshot.reflectable_property_count));
  std::printf("\"writable_property_count\":%llu,",
              static_cast<unsigned long long>(snapshot.writable_property_count));
  std::printf("\"slot_backed_property_count\":%llu,",
              static_cast<unsigned long long>(snapshot.slot_backed_property_count));
  std::printf("\"last_query_found\":%d,", snapshot.last_query_found);
  std::printf("\"last_query_inherited\":%d,", snapshot.last_query_inherited);
  std::printf("\"last_queried_class_name\":");
  PrintJsonStringOrNull(snapshot.last_queried_class_name);
  std::printf(",\"last_queried_property_name\":");
  PrintJsonStringOrNull(snapshot.last_queried_property_name);
  std::printf(",\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

void PrintPropertyEntry(const objc3_runtime_property_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"inherited\":%d,", snapshot.inherited);
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
  std::printf(",\"declaration_owner_identity\":");
  PrintJsonStringOrNull(snapshot.declaration_owner_identity);
  std::printf(",\"export_owner_identity\":");
  PrintJsonStringOrNull(snapshot.export_owner_identity);
  std::printf(",\"getter_selector\":");
  PrintJsonStringOrNull(snapshot.getter_selector);
  std::printf(",\"setter_selector\":");
  PrintJsonStringOrNull(snapshot.setter_selector);
  std::printf(",\"effective_getter_selector\":");
  PrintJsonStringOrNull(snapshot.effective_getter_selector);
  std::printf(",\"effective_setter_selector\":");
  PrintJsonStringOrNull(snapshot.effective_setter_selector);
  std::printf(",\"ivar_binding_symbol\":");
  PrintJsonStringOrNull(snapshot.ivar_binding_symbol);
  std::printf(",\"synthesized_binding_symbol\":");
  PrintJsonStringOrNull(snapshot.synthesized_binding_symbol);
  std::printf(",\"ivar_layout_symbol\":");
  PrintJsonStringOrNull(snapshot.ivar_layout_symbol);
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

}  // namespace

int main() {
  objc3_runtime_property_registry_state_snapshot registry_before{};
  objc3_runtime_property_registry_state_snapshot registry_after_count{};
  objc3_runtime_property_registry_state_snapshot registry_after_missing{};
  objc3_runtime_property_entry_snapshot token_entry{};
  objc3_runtime_property_entry_snapshot value_entry{};
  objc3_runtime_property_entry_snapshot count_entry{};
  objc3_runtime_property_entry_snapshot missing_entry{};
  objc3_runtime_property_entry_snapshot missing_class_entry{};
  objc3_runtime_realized_class_entry_snapshot widget_entry{};

  std::string registry_before_queried_class;
  std::string registry_before_queried_property;
  std::string registry_before_resolved_class;
  std::string registry_before_resolved_owner;
  std::string registry_after_count_queried_class;
  std::string registry_after_count_queried_property;
  std::string registry_after_count_resolved_class;
  std::string registry_after_count_resolved_owner;
  std::string registry_after_missing_queried_class;
  std::string registry_after_missing_queried_property;
  std::string registry_after_missing_resolved_class;
  std::string registry_after_missing_resolved_owner;

  std::string token_queried_class;
  std::string token_resolved_class;
  std::string token_property_name;
  std::string token_declaration_owner;
  std::string token_export_owner;
  std::string token_getter_selector;
  std::string token_setter_selector;
  std::string token_effective_getter_selector;
  std::string token_effective_setter_selector;
  std::string token_ivar_binding;
  std::string token_synthesized_binding;
  std::string token_layout_symbol;
  std::string token_getter_owner;
  std::string token_setter_owner;

  std::string value_queried_class;
  std::string value_resolved_class;
  std::string value_property_name;
  std::string value_declaration_owner;
  std::string value_export_owner;
  std::string value_getter_selector;
  std::string value_setter_selector;
  std::string value_effective_getter_selector;
  std::string value_effective_setter_selector;
  std::string value_ivar_binding;
  std::string value_synthesized_binding;
  std::string value_layout_symbol;
  std::string value_getter_owner;
  std::string value_setter_owner;

  std::string count_queried_class;
  std::string count_resolved_class;
  std::string count_property_name;
  std::string count_declaration_owner;
  std::string count_export_owner;
  std::string count_getter_selector;
  std::string count_setter_selector;
  std::string count_effective_getter_selector;
  std::string count_effective_setter_selector;
  std::string count_ivar_binding;
  std::string count_synthesized_binding;
  std::string count_layout_symbol;
  std::string count_getter_owner;
  std::string count_setter_owner;

  std::string missing_queried_class;
  std::string missing_resolved_class;
  std::string missing_property_name;
  std::string missing_declaration_owner;
  std::string missing_export_owner;
  std::string missing_getter_selector;
  std::string missing_setter_selector;
  std::string missing_effective_getter_selector;
  std::string missing_effective_setter_selector;
  std::string missing_ivar_binding;
  std::string missing_synthesized_binding;
  std::string missing_layout_symbol;
  std::string missing_getter_owner;
  std::string missing_setter_owner;

  std::string missing_class_queried_class;
  std::string missing_class_resolved_class;
  std::string missing_class_property_name;
  std::string missing_class_declaration_owner;
  std::string missing_class_export_owner;
  std::string missing_class_getter_selector;
  std::string missing_class_setter_selector;
  std::string missing_class_effective_getter_selector;
  std::string missing_class_effective_setter_selector;
  std::string missing_class_ivar_binding;
  std::string missing_class_synthesized_binding;
  std::string missing_class_layout_symbol;
  std::string missing_class_getter_owner;
  std::string missing_class_setter_owner;

  std::string widget_module_storage;
  std::string widget_identity_storage;
  std::string widget_class_storage;
  std::string widget_class_owner_storage;
  std::string widget_metaclass_owner_storage;
  std::string widget_super_class_storage;
  std::string widget_super_metaclass_storage;
  std::string widget_category_owner_storage;
  std::string widget_category_name_storage;

  (void)objc3_runtime_copy_property_registry_state_for_testing(&registry_before);
  StabilizePropertyRegistryState(registry_before, registry_before_queried_class,
                                 registry_before_queried_property,
                                 registry_before_resolved_class,
                                 registry_before_resolved_owner);

  (void)objc3_runtime_copy_realized_class_entry_for_testing("Widget",
                                                            &widget_entry);
  StabilizeRealizedClassEntry(widget_entry, widget_module_storage,
                              widget_identity_storage, widget_class_storage,
                              widget_class_owner_storage,
                              widget_metaclass_owner_storage,
                              widget_super_class_storage,
                              widget_super_metaclass_storage,
                              widget_category_owner_storage,
                              widget_category_name_storage);

  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "token",
                                                      &token_entry);
  StabilizePropertyEntry(token_entry, token_queried_class, token_resolved_class,
                         token_property_name, token_declaration_owner,
                         token_export_owner, token_getter_selector,
                         token_setter_selector,
                         token_effective_getter_selector,
                         token_effective_setter_selector, token_ivar_binding,
                         token_synthesized_binding, token_layout_symbol,
                         token_getter_owner, token_setter_owner);

  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "value",
                                                      &value_entry);
  StabilizePropertyEntry(value_entry, value_queried_class, value_resolved_class,
                         value_property_name, value_declaration_owner,
                         value_export_owner, value_getter_selector,
                         value_setter_selector,
                         value_effective_getter_selector,
                         value_effective_setter_selector, value_ivar_binding,
                         value_synthesized_binding, value_layout_symbol,
                         value_getter_owner, value_setter_owner);

  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "count",
                                                      &count_entry);
  StabilizePropertyEntry(count_entry, count_queried_class, count_resolved_class,
                         count_property_name, count_declaration_owner,
                         count_export_owner, count_getter_selector,
                         count_setter_selector,
                         count_effective_getter_selector,
                         count_effective_setter_selector, count_ivar_binding,
                         count_synthesized_binding, count_layout_symbol,
                         count_getter_owner, count_setter_owner);

  (void)objc3_runtime_copy_property_registry_state_for_testing(
      &registry_after_count);
  StabilizePropertyRegistryState(registry_after_count,
                                 registry_after_count_queried_class,
                                 registry_after_count_queried_property,
                                 registry_after_count_resolved_class,
                                 registry_after_count_resolved_owner);

  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "missing",
                                                      &missing_entry);
  StabilizePropertyEntry(missing_entry, missing_queried_class,
                         missing_resolved_class, missing_property_name,
                         missing_declaration_owner, missing_export_owner,
                         missing_getter_selector, missing_setter_selector,
                         missing_effective_getter_selector,
                         missing_effective_setter_selector, missing_ivar_binding,
                         missing_synthesized_binding, missing_layout_symbol,
                         missing_getter_owner, missing_setter_owner);

  (void)objc3_runtime_copy_property_entry_for_testing("MissingWidget", "count",
                                                      &missing_class_entry);
  StabilizePropertyEntry(missing_class_entry, missing_class_queried_class,
                         missing_class_resolved_class,
                         missing_class_property_name,
                         missing_class_declaration_owner,
                         missing_class_export_owner,
                         missing_class_getter_selector,
                         missing_class_setter_selector,
                         missing_class_effective_getter_selector,
                         missing_class_effective_setter_selector,
                         missing_class_ivar_binding,
                         missing_class_synthesized_binding,
                         missing_class_layout_symbol,
                         missing_class_getter_owner,
                         missing_class_setter_owner);

  (void)objc3_runtime_copy_property_registry_state_for_testing(
      &registry_after_missing);
  StabilizePropertyRegistryState(registry_after_missing,
                                 registry_after_missing_queried_class,
                                 registry_after_missing_queried_property,
                                 registry_after_missing_resolved_class,
                                 registry_after_missing_resolved_owner);

  std::printf("{");
  std::printf("\"registry_state_before\":");
  PrintPropertyRegistryState(registry_before);
  std::printf(",\"widget_entry\":");
  PrintRealizedClassEntry(widget_entry);
  std::printf(",\"token_property\":");
  PrintPropertyEntry(token_entry);
  std::printf(",\"value_property\":");
  PrintPropertyEntry(value_entry);
  std::printf(",\"count_property\":");
  PrintPropertyEntry(count_entry);
  std::printf(",\"registry_state_after_count\":");
  PrintPropertyRegistryState(registry_after_count);
  std::printf(",\"missing_property\":");
  PrintPropertyEntry(missing_entry);
  std::printf(",\"missing_class_property\":");
  PrintPropertyEntry(missing_class_entry);
  std::printf(",\"registry_state_after_missing\":");
  PrintPropertyRegistryState(registry_after_missing);
  std::printf("}");
  return 0;
}
