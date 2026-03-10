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

void StabilizeRealizedGraphState(
    objc3_runtime_realized_class_graph_state_snapshot &snapshot,
    std::string &class_storage, std::string &owner_storage,
    std::string &metaclass_storage, std::string &category_owner_storage,
    std::string &category_name_storage, std::string &allocated_class_storage) {
  StabilizeNullableCString(snapshot.last_realized_class_name, class_storage,
                           snapshot.last_realized_class_name);
  StabilizeNullableCString(snapshot.last_realized_class_owner_identity,
                           owner_storage,
                           snapshot.last_realized_class_owner_identity);
  StabilizeNullableCString(snapshot.last_realized_metaclass_owner_identity,
                           metaclass_storage,
                           snapshot.last_realized_metaclass_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_owner_identity,
                           category_owner_storage,
                           snapshot.last_attached_category_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_name,
                           category_name_storage,
                           snapshot.last_attached_category_name);
  StabilizeNullableCString(snapshot.last_allocated_class_name,
                           allocated_class_storage,
                           snapshot.last_allocated_class_name);
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

void PrintRegistrationState(
    const objc3_runtime_registration_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"registered_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_image_count));
  std::printf("\"registered_descriptor_total\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_descriptor_total));
  std::printf("\"last_registration_status\":%d,",
              snapshot.last_registration_status);
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

void PrintRealizedGraphState(
    const objc3_runtime_realized_class_graph_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"realized_class_count\":%llu,",
              static_cast<unsigned long long>(snapshot.realized_class_count));
  std::printf("\"root_class_count\":%llu,",
              static_cast<unsigned long long>(snapshot.root_class_count));
  std::printf("\"metaclass_edge_count\":%llu,",
              static_cast<unsigned long long>(snapshot.metaclass_edge_count));
  std::printf("\"receiver_class_binding_count\":%llu,",
              static_cast<unsigned long long>(snapshot.receiver_class_binding_count));
  std::printf("\"attached_category_count\":%llu,",
              static_cast<unsigned long long>(snapshot.attached_category_count));
  std::printf("\"protocol_conformance_edge_count\":%llu,",
              static_cast<unsigned long long>(snapshot.protocol_conformance_edge_count));
  std::printf("\"live_instance_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_instance_count));
  std::printf("\"last_allocated_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_allocated_receiver_identity));
  std::printf("\"last_allocated_base_identity\":%llu,",
              static_cast<unsigned long long>(snapshot.last_allocated_base_identity));
  std::printf("\"last_allocated_instance_size_bytes\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_allocated_instance_size_bytes));
  std::printf("\"last_realized_class_name\":");
  PrintJsonStringOrNull(snapshot.last_realized_class_name);
  std::printf(",\"last_realized_class_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_realized_class_owner_identity);
  std::printf(",\"last_realized_metaclass_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_realized_metaclass_owner_identity);
  std::printf(",\"last_allocated_class_name\":");
  PrintJsonStringOrNull(snapshot.last_allocated_class_name);
  std::printf("}");
}

void PrintRealizedClassEntry(
    const objc3_runtime_realized_class_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"base_identity\":%llu,",
              static_cast<unsigned long long>(snapshot.base_identity));
  std::printf("\"registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(snapshot.registration_order_ordinal));
  std::printf("\"is_root_class\":%d,", snapshot.is_root_class);
  std::printf("\"implementation_backed\":%d,", snapshot.implementation_backed);
  std::printf("\"attached_category_count\":%llu,",
              static_cast<unsigned long long>(snapshot.attached_category_count));
  std::printf("\"direct_protocol_count\":%llu,",
              static_cast<unsigned long long>(snapshot.direct_protocol_count));
  std::printf("\"attached_protocol_count\":%llu,",
              static_cast<unsigned long long>(snapshot.attached_protocol_count));
  std::printf("\"runtime_property_accessor_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.runtime_property_accessor_count));
  std::printf("\"runtime_instance_size_bytes\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.runtime_instance_size_bytes));
  std::printf("\"class_name\":");
  PrintJsonStringOrNull(snapshot.class_name);
  std::printf(",\"class_owner_identity\":");
  PrintJsonStringOrNull(snapshot.class_owner_identity);
  std::printf(",\"metaclass_owner_identity\":");
  PrintJsonStringOrNull(snapshot.metaclass_owner_identity);
  std::printf("}");
}

}  // namespace

int main() {
  objc3_runtime_registration_state_snapshot registration_state{};
  objc3_runtime_selector_lookup_table_state_snapshot selector_table_state{};
  objc3_runtime_method_cache_entry_snapshot count_entry{};
  objc3_runtime_method_cache_entry_snapshot set_count_entry{};
  objc3_runtime_realized_class_graph_state_snapshot graph_state{};
  objc3_runtime_realized_class_entry_snapshot widget_entry{};

  std::string registration_module_storage;
  std::string registration_identity_storage;
  std::string selector_table_last_storage;
  std::string count_selector_storage;
  std::string count_class_storage;
  std::string count_owner_storage;
  std::string set_count_selector_storage;
  std::string set_count_class_storage;
  std::string set_count_owner_storage;
  std::string graph_class_storage;
  std::string graph_owner_storage;
  std::string graph_metaclass_storage;
  std::string graph_category_owner_storage;
  std::string graph_category_name_storage;
  std::string graph_allocated_class_storage;
  std::string widget_module_storage;
  std::string widget_identity_storage;
  std::string widget_class_storage;
  std::string widget_class_owner_storage;
  std::string widget_metaclass_owner_storage;
  std::string widget_super_class_storage;
  std::string widget_super_metaclass_storage;
  std::string widget_category_owner_storage;
  std::string widget_category_name_storage;

  (void)objc3_runtime_copy_registration_state_for_testing(&registration_state);
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &selector_table_state);
  StabilizeRegistrationState(registration_state, registration_module_storage,
                             registration_identity_storage);
  StabilizeSelectorTableState(selector_table_state, selector_table_last_storage);

  const int first_alloc = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  const int second_alloc = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);

  const int set_count_first =
      objc3_runtime_dispatch_i32(first_alloc, "setCount:", 37, 0, 0, 0);
  const int count_value_first =
      objc3_runtime_dispatch_i32(first_alloc, "count", 0, 0, 0, 0);
  const int count_value_second_before =
      objc3_runtime_dispatch_i32(second_alloc, "count", 0, 0, 0, 0);

  const int set_enabled_first =
      objc3_runtime_dispatch_i32(first_alloc, "setEnabled:", 1, 0, 0, 0);
  const int enabled_value_first =
      objc3_runtime_dispatch_i32(first_alloc, "enabled", 0, 0, 0, 0);
  const int enabled_value_second =
      objc3_runtime_dispatch_i32(second_alloc, "enabled", 0, 0, 0, 0);

  const int set_value_first =
      objc3_runtime_dispatch_i32(first_alloc, "setValue:", 55, 0, 0, 0);
  const int value_result_first =
      objc3_runtime_dispatch_i32(first_alloc, "value", 0, 0, 0, 0);
  const int value_result_second_before =
      objc3_runtime_dispatch_i32(second_alloc, "value", 0, 0, 0, 0);

  const int set_count_second =
      objc3_runtime_dispatch_i32(second_alloc, "setCount:", 9, 0, 0, 0);
  const int count_value_first_after_second =
      objc3_runtime_dispatch_i32(first_alloc, "count", 0, 0, 0, 0);
  const int count_value_second_after =
      objc3_runtime_dispatch_i32(second_alloc, "count", 0, 0, 0, 0);

  const int set_value_second =
      objc3_runtime_dispatch_i32(second_alloc, "setValue:", 91, 0, 0, 0);
  const int value_result_first_after_second =
      objc3_runtime_dispatch_i32(first_alloc, "value", 0, 0, 0, 0);
  const int value_result_second_after =
      objc3_runtime_dispatch_i32(second_alloc, "value", 0, 0, 0, 0);

  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      first_alloc, "count", &count_entry);
  StabilizeMethodCacheEntry(count_entry, count_selector_storage,
                            count_class_storage, count_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      first_alloc, "setCount:", &set_count_entry);
  StabilizeMethodCacheEntry(set_count_entry, set_count_selector_storage,
                            set_count_class_storage, set_count_owner_storage);

  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(&graph_state);
  StabilizeRealizedGraphState(graph_state, graph_class_storage, graph_owner_storage,
                              graph_metaclass_storage,
                              graph_category_owner_storage,
                              graph_category_name_storage,
                              graph_allocated_class_storage);
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

  std::printf("{");
  std::printf("\"first_alloc\":%d,", first_alloc);
  std::printf("\"second_alloc\":%d,", second_alloc);
  std::printf("\"set_count_first\":%d,", set_count_first);
  std::printf("\"count_value_first\":%d,", count_value_first);
  std::printf("\"count_value_second_before\":%d,",
              count_value_second_before);
  std::printf("\"set_enabled_first\":%d,", set_enabled_first);
  std::printf("\"enabled_value_first\":%d,", enabled_value_first);
  std::printf("\"enabled_value_second\":%d,", enabled_value_second);
  std::printf("\"set_value_first\":%d,", set_value_first);
  std::printf("\"value_result_first\":%d,", value_result_first);
  std::printf("\"value_result_second_before\":%d,",
              value_result_second_before);
  std::printf("\"set_count_second\":%d,", set_count_second);
  std::printf("\"count_value_first_after_second\":%d,",
              count_value_first_after_second);
  std::printf("\"count_value_second_after\":%d,",
              count_value_second_after);
  std::printf("\"set_value_second\":%d,", set_value_second);
  std::printf("\"value_result_first_after_second\":%d,",
              value_result_first_after_second);
  std::printf("\"value_result_second_after\":%d,",
              value_result_second_after);
  std::printf("\"registration_state\":");
  PrintRegistrationState(registration_state);
  std::printf(",\"selector_table_state\":");
  PrintSelectorTableState(selector_table_state);
  std::printf(",\"graph_state\":");
  PrintRealizedGraphState(graph_state);
  std::printf(",\"widget_entry\":");
  PrintRealizedClassEntry(widget_entry);
  std::printf(",\"count_entry\":");
  PrintMethodCacheEntry(count_entry);
  std::printf(",\"set_count_entry\":");
  PrintMethodCacheEntry(set_count_entry);
  std::printf("}");
  return 0;
}
