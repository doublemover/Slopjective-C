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

void StabilizeConformanceQuery(
    objc3_runtime_protocol_conformance_query_snapshot &snapshot,
    std::string &class_storage, std::string &protocol_storage,
    std::string &protocol_owner_storage,
    std::string &attachment_owner_storage) {
  StabilizeNullableCString(snapshot.class_name, class_storage,
                           snapshot.class_name);
  StabilizeNullableCString(snapshot.protocol_name, protocol_storage,
                           snapshot.protocol_name);
  StabilizeNullableCString(snapshot.matched_protocol_owner_identity,
                           protocol_owner_storage,
                           snapshot.matched_protocol_owner_identity);
  StabilizeNullableCString(snapshot.matched_attachment_owner_identity,
                           attachment_owner_storage,
                           snapshot.matched_attachment_owner_identity);
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
              static_cast<unsigned long long>(
                  snapshot.runtime_instance_size_bytes));
  std::printf("\"class_name\":");
  PrintJsonStringOrNull(snapshot.class_name);
  std::printf(",\"class_owner_identity\":");
  PrintJsonStringOrNull(snapshot.class_owner_identity);
  std::printf(",\"last_attached_category_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_attached_category_owner_identity);
  std::printf("}");
}

void PrintConformanceQuery(
    const objc3_runtime_protocol_conformance_query_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"conforms\":%d,", snapshot.conforms);
  std::printf("\"class_name\":");
  PrintJsonStringOrNull(snapshot.class_name);
  std::printf(",\"protocol_name\":");
  PrintJsonStringOrNull(snapshot.protocol_name);
  std::printf(",\"matched_protocol_owner_identity\":");
  PrintJsonStringOrNull(snapshot.matched_protocol_owner_identity);
  std::printf(",\"matched_attachment_owner_identity\":");
  PrintJsonStringOrNull(snapshot.matched_attachment_owner_identity);
  std::printf("}");
}

void PrintPropertyEntry(const objc3_runtime_property_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"setter_available\":%d,", snapshot.setter_available);
  std::printf("\"has_runtime_getter\":%d,", snapshot.has_runtime_getter);
  std::printf("\"has_runtime_setter\":%d,", snapshot.has_runtime_setter);
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
  std::printf("\"property_name\":");
  PrintJsonStringOrNull(snapshot.property_name);
  std::printf(",\"effective_getter_selector\":");
  PrintJsonStringOrNull(snapshot.effective_getter_selector);
  std::printf(",\"effective_setter_selector\":");
  PrintJsonStringOrNull(snapshot.effective_setter_selector);
  std::printf(",\"getter_owner_identity\":");
  PrintJsonStringOrNull(snapshot.getter_owner_identity);
  std::printf(",\"setter_owner_identity\":");
  PrintJsonStringOrNull(snapshot.setter_owner_identity);
  std::printf("}");
}

}  // namespace

int main() {
  objc3_runtime_realized_class_entry_snapshot widget_entry{};
  (void)objc3_runtime_copy_realized_class_entry_for_testing("Widget",
                                                            &widget_entry);

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

  const int widget_class_receiver =
      static_cast<int>(widget_entry.base_identity + 2U);
  const int widget_instance =
      objc3_runtime_dispatch_i32(widget_class_receiver, "alloc", 0, 0, 0, 0);
  const int init_value =
      objc3_runtime_dispatch_i32(widget_instance, "init", 0, 0, 0, 0);
  const int traced_value =
      objc3_runtime_dispatch_i32(init_value, "tracedValue", 0, 0, 0, 0);
  const int inherited_value =
      objc3_runtime_dispatch_i32(init_value, "inheritedValue", 0, 0, 0, 0);
  const int class_value =
      objc3_runtime_dispatch_i32(widget_class_receiver, "classValue", 0, 0, 0,
                                 0);
  const int shared_value =
      objc3_runtime_dispatch_i32(widget_class_receiver, "shared", 0, 0, 0, 0);
  (void)objc3_runtime_dispatch_i32(init_value, "setCount:", 37, 0, 0, 0);
  const int count_value =
      objc3_runtime_dispatch_i32(init_value, "count", 0, 0, 0, 0);
  (void)objc3_runtime_dispatch_i32(init_value, "setEnabled:", 1, 0, 0, 0);
  const int enabled_value =
      objc3_runtime_dispatch_i32(init_value, "enabled", 0, 0, 0, 0);
  (void)objc3_runtime_dispatch_i32(init_value, "setCurrentValue:", 55, 0, 0,
                                   0);
  const int current_value =
      objc3_runtime_dispatch_i32(init_value, "currentValue", 0, 0, 0, 0);
  const int token_value =
      objc3_runtime_dispatch_i32(init_value, "tokenValue", 0, 0, 0, 0);

  objc3_runtime_protocol_conformance_query_snapshot worker_query{};
  objc3_runtime_protocol_conformance_query_snapshot tracer_query{};
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Widget", "Worker", &worker_query);
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Widget", "Tracer", &tracer_query);

  std::string worker_class_storage;
  std::string worker_protocol_storage;
  std::string worker_protocol_owner_storage;
  std::string worker_attachment_owner_storage;
  std::string tracer_class_storage;
  std::string tracer_protocol_storage;
  std::string tracer_protocol_owner_storage;
  std::string tracer_attachment_owner_storage;
  StabilizeConformanceQuery(worker_query, worker_class_storage,
                            worker_protocol_storage,
                            worker_protocol_owner_storage,
                            worker_attachment_owner_storage);
  StabilizeConformanceQuery(tracer_query, tracer_class_storage,
                            tracer_protocol_storage,
                            tracer_protocol_owner_storage,
                            tracer_attachment_owner_storage);

  objc3_runtime_property_entry_snapshot count_property{};
  objc3_runtime_property_entry_snapshot value_property{};
  objc3_runtime_property_entry_snapshot token_property{};
  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "count",
                                                      &count_property);
  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "value",
                                                      &value_property);
  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "token",
                                                      &token_property);

  std::string property_storage[45];
  StabilizePropertyEntry(
      count_property, property_storage[0], property_storage[1],
      property_storage[2], property_storage[3], property_storage[4],
      property_storage[5], property_storage[6], property_storage[7],
      property_storage[8], property_storage[9], property_storage[10],
      property_storage[11], property_storage[12], property_storage[13]);
  StabilizePropertyEntry(
      value_property, property_storage[14], property_storage[15],
      property_storage[16], property_storage[17], property_storage[18],
      property_storage[19], property_storage[20], property_storage[21],
      property_storage[22], property_storage[23], property_storage[24],
      property_storage[25], property_storage[26], property_storage[27]);
  StabilizePropertyEntry(
      token_property, property_storage[28], property_storage[29],
      property_storage[30], property_storage[31], property_storage[32],
      property_storage[33], property_storage[34], property_storage[35],
      property_storage[36], property_storage[37], property_storage[38],
      property_storage[39], property_storage[40], property_storage[41]);

  std::printf("{");
  std::printf("\"widget_entry\":");
  PrintRealizedClassEntry(widget_entry);
  std::printf(",\"init_value\":%d", init_value);
  std::printf(",\"traced_value\":%d", traced_value);
  std::printf(",\"inherited_value\":%d", inherited_value);
  std::printf(",\"class_value\":%d", class_value);
  std::printf(",\"shared_value\":%d", shared_value);
  std::printf(",\"count_value\":%d", count_value);
  std::printf(",\"enabled_value\":%d", enabled_value);
  std::printf(",\"current_value\":%d", current_value);
  std::printf(",\"token_value\":%d", token_value);
  std::printf(",\"worker_query\":");
  PrintConformanceQuery(worker_query);
  std::printf(",\"tracer_query\":");
  PrintConformanceQuery(tracer_query);
  std::printf(",\"count_property\":");
  PrintPropertyEntry(count_property);
  std::printf(",\"value_property\":");
  PrintPropertyEntry(value_property);
  std::printf(",\"token_property\":");
  PrintPropertyEntry(token_property);
  std::printf("}\n");
  return 0;
}
