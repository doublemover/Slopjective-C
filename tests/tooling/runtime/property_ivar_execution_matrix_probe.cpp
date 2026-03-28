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

void StabilizeDispatchState(
    objc3_runtime_dispatch_state_snapshot &snapshot,
    std::string &selector_storage, std::string &fast_path_reason_storage,
    std::string &dispatch_path_storage,
    std::string &implementation_kind_storage,
    std::string &property_name_storage,
    std::string &resolved_class_storage,
    std::string &resolved_owner_storage) {
  StabilizeNullableCString(snapshot.last_selector, selector_storage,
                           snapshot.last_selector);
  StabilizeNullableCString(snapshot.last_fast_path_reason,
                           fast_path_reason_storage,
                           snapshot.last_fast_path_reason);
  StabilizeNullableCString(snapshot.last_dispatch_path, dispatch_path_storage,
                           snapshot.last_dispatch_path);
  StabilizeNullableCString(snapshot.last_implementation_kind,
                           implementation_kind_storage,
                           snapshot.last_implementation_kind);
  StabilizeNullableCString(snapshot.last_property_name, property_name_storage,
                           snapshot.last_property_name);
  StabilizeNullableCString(snapshot.last_resolved_class_name,
                           resolved_class_storage,
                           snapshot.last_resolved_class_name);
  StabilizeNullableCString(snapshot.last_resolved_owner_identity,
                           resolved_owner_storage,
                           snapshot.last_resolved_owner_identity);
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

void PrintDispatchState(
    const objc3_runtime_dispatch_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"cache_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_entry_count));
  std::printf("\"fast_path_seed_count\":%llu,",
              static_cast<unsigned long long>(snapshot.fast_path_seed_count));
  std::printf("\"fast_path_hit_count\":%llu,",
              static_cast<unsigned long long>(snapshot.fast_path_hit_count));
  std::printf("\"live_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_dispatch_count));
  std::printf("\"fallback_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.fallback_dispatch_count));
  std::printf("\"last_resolved_parameter_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_resolved_parameter_count));
  std::printf("\"last_property_base_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_property_base_identity));
  std::printf("\"last_property_slot_index\":%llu,",
              static_cast<unsigned long long>(snapshot.last_property_slot_index));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_used_fast_path\":%d,",
              snapshot.last_dispatch_used_fast_path);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_fell_back\":%d,",
              snapshot.last_dispatch_fell_back);
  std::printf("\"last_effective_direct_dispatch\":%d,",
              snapshot.last_effective_direct_dispatch);
  std::printf("\"last_used_builtin\":%d,", snapshot.last_used_builtin);
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(",\"last_fast_path_reason\":");
  PrintJsonStringOrNull(snapshot.last_fast_path_reason);
  std::printf(",\"last_dispatch_path\":");
  PrintJsonStringOrNull(snapshot.last_dispatch_path);
  std::printf(",\"last_implementation_kind\":");
  PrintJsonStringOrNull(snapshot.last_implementation_kind);
  std::printf(",\"last_property_name\":");
  PrintJsonStringOrNull(snapshot.last_property_name);
  std::printf(",\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

}  // namespace

int main() {
  std::string set_count_dispatch_selector;
  std::string set_count_dispatch_fast_path_reason;
  std::string set_count_dispatch_path;
  std::string set_count_dispatch_implementation_kind;
  std::string set_count_dispatch_property_name;
  std::string set_count_dispatch_class_name;
  std::string set_count_dispatch_owner_identity;
  std::string count_dispatch_selector;
  std::string count_dispatch_fast_path_reason;
  std::string count_dispatch_path;
  std::string count_dispatch_implementation_kind;
  std::string count_dispatch_property_name;
  std::string count_dispatch_class_name;
  std::string count_dispatch_owner_identity;
  std::string set_enabled_dispatch_selector;
  std::string set_enabled_dispatch_fast_path_reason;
  std::string set_enabled_dispatch_path;
  std::string set_enabled_dispatch_implementation_kind;
  std::string set_enabled_dispatch_property_name;
  std::string set_enabled_dispatch_class_name;
  std::string set_enabled_dispatch_owner_identity;
  std::string enabled_dispatch_selector;
  std::string enabled_dispatch_fast_path_reason;
  std::string enabled_dispatch_path;
  std::string enabled_dispatch_implementation_kind;
  std::string enabled_dispatch_property_name;
  std::string enabled_dispatch_class_name;
  std::string enabled_dispatch_owner_identity;
  std::string set_value_dispatch_selector;
  std::string set_value_dispatch_fast_path_reason;
  std::string set_value_dispatch_path;
  std::string set_value_dispatch_implementation_kind;
  std::string set_value_dispatch_property_name;
  std::string set_value_dispatch_class_name;
  std::string set_value_dispatch_owner_identity;
  std::string value_dispatch_selector;
  std::string value_dispatch_fast_path_reason;
  std::string value_dispatch_path;
  std::string value_dispatch_implementation_kind;
  std::string value_dispatch_property_name;
  std::string value_dispatch_class_name;
  std::string value_dispatch_owner_identity;
  std::string token_dispatch_selector;
  std::string token_dispatch_fast_path_reason;
  std::string token_dispatch_path;
  std::string token_dispatch_implementation_kind;
  std::string token_dispatch_property_name;
  std::string token_dispatch_class_name;
  std::string token_dispatch_owner_identity;
  const int widget_instance = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  objc3_runtime_dispatch_state_snapshot set_count_dispatch{};
  const int set_count_result =
      objc3_runtime_dispatch_i32(widget_instance, "setCount:", 37, 0, 0, 0);
  (void)objc3_runtime_copy_dispatch_state_for_testing(&set_count_dispatch);
  StabilizeDispatchState(
      set_count_dispatch, set_count_dispatch_selector,
      set_count_dispatch_fast_path_reason, set_count_dispatch_path,
      set_count_dispatch_implementation_kind,
      set_count_dispatch_property_name, set_count_dispatch_class_name,
      set_count_dispatch_owner_identity);
  objc3_runtime_dispatch_state_snapshot count_dispatch{};
  const int count_value =
      objc3_runtime_dispatch_i32(widget_instance, "count", 0, 0, 0, 0);
  (void)objc3_runtime_copy_dispatch_state_for_testing(&count_dispatch);
  StabilizeDispatchState(
      count_dispatch, count_dispatch_selector, count_dispatch_fast_path_reason,
      count_dispatch_path, count_dispatch_implementation_kind,
      count_dispatch_property_name, count_dispatch_class_name,
      count_dispatch_owner_identity);
  objc3_runtime_dispatch_state_snapshot set_enabled_dispatch{};
  const int set_enabled_result =
      objc3_runtime_dispatch_i32(widget_instance, "setEnabled:", 1, 0, 0, 0);
  (void)objc3_runtime_copy_dispatch_state_for_testing(&set_enabled_dispatch);
  StabilizeDispatchState(
      set_enabled_dispatch, set_enabled_dispatch_selector,
      set_enabled_dispatch_fast_path_reason, set_enabled_dispatch_path,
      set_enabled_dispatch_implementation_kind,
      set_enabled_dispatch_property_name, set_enabled_dispatch_class_name,
      set_enabled_dispatch_owner_identity);
  objc3_runtime_dispatch_state_snapshot enabled_dispatch{};
  const int enabled_value =
      objc3_runtime_dispatch_i32(widget_instance, "enabled", 0, 0, 0, 0);
  (void)objc3_runtime_copy_dispatch_state_for_testing(&enabled_dispatch);
  StabilizeDispatchState(
      enabled_dispatch, enabled_dispatch_selector,
      enabled_dispatch_fast_path_reason, enabled_dispatch_path,
      enabled_dispatch_implementation_kind, enabled_dispatch_property_name,
      enabled_dispatch_class_name, enabled_dispatch_owner_identity);
  objc3_runtime_dispatch_state_snapshot set_value_dispatch{};
  const int set_value_result =
      objc3_runtime_dispatch_i32(widget_instance, "setCurrentValue:", 55, 0, 0, 0);
  (void)objc3_runtime_copy_dispatch_state_for_testing(&set_value_dispatch);
  StabilizeDispatchState(
      set_value_dispatch, set_value_dispatch_selector,
      set_value_dispatch_fast_path_reason, set_value_dispatch_path,
      set_value_dispatch_implementation_kind,
      set_value_dispatch_property_name, set_value_dispatch_class_name,
      set_value_dispatch_owner_identity);
  objc3_runtime_dispatch_state_snapshot value_dispatch{};
  const int value_result =
      objc3_runtime_dispatch_i32(widget_instance, "currentValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_dispatch_state_for_testing(&value_dispatch);
  StabilizeDispatchState(
      value_dispatch, value_dispatch_selector, value_dispatch_fast_path_reason,
      value_dispatch_path, value_dispatch_implementation_kind,
      value_dispatch_property_name, value_dispatch_class_name,
      value_dispatch_owner_identity);
  objc3_runtime_dispatch_state_snapshot token_dispatch{};
  const int token_value =
      objc3_runtime_dispatch_i32(widget_instance, "tokenValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_dispatch_state_for_testing(&token_dispatch);
  StabilizeDispatchState(
      token_dispatch, token_dispatch_selector, token_dispatch_fast_path_reason,
      token_dispatch_path, token_dispatch_implementation_kind,
      token_dispatch_property_name, token_dispatch_class_name,
      token_dispatch_owner_identity);

  objc3_runtime_realized_class_entry_snapshot widget_entry{};
  (void)objc3_runtime_copy_realized_class_entry_for_testing("Widget", &widget_entry);
  std::string widget_module_storage;
  std::string widget_identity_storage;
  std::string widget_class_storage;
  std::string widget_class_owner_storage;
  std::string widget_metaclass_owner_storage;
  std::string widget_super_class_storage;
  std::string widget_super_metaclass_storage;
  std::string widget_category_owner_storage;
  std::string widget_category_name_storage;
  StabilizeRealizedClassEntry(
      widget_entry, widget_module_storage, widget_identity_storage,
      widget_class_storage, widget_class_owner_storage,
      widget_metaclass_owner_storage, widget_super_class_storage,
      widget_super_metaclass_storage, widget_category_owner_storage,
      widget_category_name_storage);

  objc3_runtime_property_entry_snapshot count_entry{};
  objc3_runtime_property_entry_snapshot enabled_entry{};
  objc3_runtime_property_entry_snapshot value_entry{};
  objc3_runtime_property_entry_snapshot token_entry{};
  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "count", &count_entry);
  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "enabled", &enabled_entry);
  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "value", &value_entry);
  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "token", &token_entry);

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
  StabilizePropertyEntry(
      count_entry, count_queried_class, count_resolved_class, count_property_name,
      count_declaration_owner, count_export_owner, count_getter_selector,
      count_setter_selector, count_effective_getter_selector,
      count_effective_setter_selector, count_ivar_binding,
      count_synthesized_binding, count_layout_symbol, count_getter_owner,
      count_setter_owner);

  std::string enabled_queried_class;
  std::string enabled_resolved_class;
  std::string enabled_property_name;
  std::string enabled_declaration_owner;
  std::string enabled_export_owner;
  std::string enabled_getter_selector;
  std::string enabled_setter_selector;
  std::string enabled_effective_getter_selector;
  std::string enabled_effective_setter_selector;
  std::string enabled_ivar_binding;
  std::string enabled_synthesized_binding;
  std::string enabled_layout_symbol;
  std::string enabled_getter_owner;
  std::string enabled_setter_owner;
  StabilizePropertyEntry(
      enabled_entry, enabled_queried_class, enabled_resolved_class,
      enabled_property_name, enabled_declaration_owner, enabled_export_owner,
      enabled_getter_selector, enabled_setter_selector,
      enabled_effective_getter_selector, enabled_effective_setter_selector,
      enabled_ivar_binding, enabled_synthesized_binding, enabled_layout_symbol,
      enabled_getter_owner, enabled_setter_owner);

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
  StabilizePropertyEntry(
      value_entry, value_queried_class, value_resolved_class, value_property_name,
      value_declaration_owner, value_export_owner, value_getter_selector,
      value_setter_selector, value_effective_getter_selector,
      value_effective_setter_selector, value_ivar_binding,
      value_synthesized_binding, value_layout_symbol, value_getter_owner,
      value_setter_owner);

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
  StabilizePropertyEntry(
      token_entry, token_queried_class, token_resolved_class, token_property_name,
      token_declaration_owner, token_export_owner, token_getter_selector,
      token_setter_selector, token_effective_getter_selector,
      token_effective_setter_selector, token_ivar_binding,
      token_synthesized_binding, token_layout_symbol, token_getter_owner,
      token_setter_owner);

  objc3_runtime_property_registry_state_snapshot registry_state{};
  (void)objc3_runtime_copy_property_registry_state_for_testing(&registry_state);
  std::string registry_queried_class;
  std::string registry_queried_property;
  std::string registry_resolved_class;
  std::string registry_resolved_owner;
  StabilizePropertyRegistryState(
      registry_state, registry_queried_class, registry_queried_property,
      registry_resolved_class, registry_resolved_owner);

  objc3_runtime_method_cache_entry_snapshot count_method{};
  objc3_runtime_method_cache_entry_snapshot enabled_method{};
  objc3_runtime_method_cache_entry_snapshot value_method{};
  objc3_runtime_method_cache_entry_snapshot token_method{};
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      widget_instance, "count", &count_method);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      widget_instance, "enabled", &enabled_method);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      widget_instance, "currentValue", &value_method);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      widget_instance, "tokenValue", &token_method);

  std::string count_method_selector;
  std::string count_method_class;
  std::string count_method_owner;
  StabilizeMethodCacheEntry(count_method, count_method_selector, count_method_class,
                            count_method_owner);
  std::string enabled_method_selector;
  std::string enabled_method_class;
  std::string enabled_method_owner;
  StabilizeMethodCacheEntry(enabled_method, enabled_method_selector,
                            enabled_method_class, enabled_method_owner);
  std::string value_method_selector;
  std::string value_method_class;
  std::string value_method_owner;
  StabilizeMethodCacheEntry(value_method, value_method_selector, value_method_class,
                            value_method_owner);
  std::string token_method_selector;
  std::string token_method_class;
  std::string token_method_owner;
  StabilizeMethodCacheEntry(token_method, token_method_selector, token_method_class,
                            token_method_owner);

  std::printf("{");
  std::printf("\"widget_instance\":%d,", widget_instance);
  std::printf("\"set_count_result\":%d,", set_count_result);
  std::printf("\"count_value\":%d,", count_value);
  std::printf("\"set_enabled_result\":%d,", set_enabled_result);
  std::printf("\"enabled_value\":%d,", enabled_value);
  std::printf("\"set_value_result\":%d,", set_value_result);
  std::printf("\"value_result\":%d,", value_result);
  std::printf("\"token_value\":%d,", token_value);
  std::printf("\"widget_entry\":");
  PrintRealizedClassEntry(widget_entry);
  std::printf(",\"registry_state\":");
  PrintPropertyRegistryState(registry_state);
  std::printf(",\"count_property\":");
  PrintPropertyEntry(count_entry);
  std::printf(",\"enabled_property\":");
  PrintPropertyEntry(enabled_entry);
  std::printf(",\"value_property\":");
  PrintPropertyEntry(value_entry);
  std::printf(",\"token_property\":");
  PrintPropertyEntry(token_entry);
  std::printf(",\"count_method\":");
  PrintMethodCacheEntry(count_method);
  std::printf(",\"enabled_method\":");
  PrintMethodCacheEntry(enabled_method);
  std::printf(",\"value_method\":");
  PrintMethodCacheEntry(value_method);
  std::printf(",\"token_method\":");
  PrintMethodCacheEntry(token_method);
  std::printf(",\"set_count_dispatch\":");
  PrintDispatchState(set_count_dispatch);
  std::printf(",\"count_dispatch\":");
  PrintDispatchState(count_dispatch);
  std::printf(",\"set_enabled_dispatch\":");
  PrintDispatchState(set_enabled_dispatch);
  std::printf(",\"enabled_dispatch\":");
  PrintDispatchState(enabled_dispatch);
  std::printf(",\"set_value_dispatch\":");
  PrintDispatchState(set_value_dispatch);
  std::printf(",\"value_dispatch\":");
  PrintDispatchState(value_dispatch);
  std::printf(",\"token_dispatch\":");
  PrintDispatchState(token_dispatch);
  std::printf("}");
  return 0;
}
