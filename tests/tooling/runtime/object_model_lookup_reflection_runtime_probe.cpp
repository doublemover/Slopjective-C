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

void StabilizeAggregateSnapshot(
    objc3_runtime_object_model_query_state_snapshot &snapshot,
    std::string &queried_class_storage, std::string &resolved_class_storage,
    std::string &resolved_class_owner_storage,
    std::string &queried_property_storage,
    std::string &resolved_property_class_storage,
    std::string &resolved_property_owner_storage,
    std::string &queried_protocol_class_storage,
    std::string &queried_protocol_storage,
    std::string &matched_protocol_owner_storage,
    std::string &matched_attachment_owner_storage) {
  StabilizeNullableCString(snapshot.last_queried_class_name,
                           queried_class_storage,
                           snapshot.last_queried_class_name);
  StabilizeNullableCString(snapshot.last_resolved_class_name,
                           resolved_class_storage,
                           snapshot.last_resolved_class_name);
  StabilizeNullableCString(snapshot.last_resolved_class_owner_identity,
                           resolved_class_owner_storage,
                           snapshot.last_resolved_class_owner_identity);
  StabilizeNullableCString(snapshot.last_queried_property_name,
                           queried_property_storage,
                           snapshot.last_queried_property_name);
  StabilizeNullableCString(snapshot.last_resolved_property_class_name,
                           resolved_property_class_storage,
                           snapshot.last_resolved_property_class_name);
  StabilizeNullableCString(snapshot.last_resolved_property_owner_identity,
                           resolved_property_owner_storage,
                           snapshot.last_resolved_property_owner_identity);
  StabilizeNullableCString(snapshot.last_queried_protocol_class_name,
                           queried_protocol_class_storage,
                           snapshot.last_queried_protocol_class_name);
  StabilizeNullableCString(snapshot.last_queried_protocol_name,
                           queried_protocol_storage,
                           snapshot.last_queried_protocol_name);
  StabilizeNullableCString(snapshot.last_matched_protocol_owner_identity,
                           matched_protocol_owner_storage,
                           snapshot.last_matched_protocol_owner_identity);
  StabilizeNullableCString(snapshot.last_matched_attachment_owner_identity,
                           matched_attachment_owner_storage,
                           snapshot.last_matched_attachment_owner_identity);
}

}  // namespace

int main() {
  objc3_runtime_realized_class_entry_snapshot widget_entry{};
  (void)objc3_runtime_copy_realized_class_entry_for_testing("Widget",
                                                            &widget_entry);
  const int widget_class_receiver =
      static_cast<int>(widget_entry.base_identity + 2U);
  const int widget_instance =
      objc3_runtime_dispatch_i32(widget_class_receiver, "alloc", 0, 0, 0, 0);
  const int init_value =
      objc3_runtime_dispatch_i32(widget_instance, "init", 0, 0, 0, 0);
  const int traced_value =
      objc3_runtime_dispatch_i32(init_value, "tracedValue", 0, 0, 0, 0);
  (void)objc3_runtime_dispatch_i32(init_value, "setCount:", 41, 0, 0, 0);
  const int count_value =
      objc3_runtime_dispatch_i32(init_value, "count", 0, 0, 0, 0);

  objc3_runtime_property_entry_snapshot count_property{};
  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "count",
                                                      &count_property);

  objc3_runtime_protocol_conformance_query_snapshot tracer_query{};
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Widget", "Tracer", &tracer_query);

  objc3_runtime_object_model_query_state_snapshot aggregate{};
  (void)objc3_runtime_copy_object_model_query_state_for_testing(&aggregate);

  std::string aggregate_storage[10];
  StabilizeAggregateSnapshot(
      aggregate, aggregate_storage[0], aggregate_storage[1],
      aggregate_storage[2], aggregate_storage[3], aggregate_storage[4],
      aggregate_storage[5], aggregate_storage[6], aggregate_storage[7],
      aggregate_storage[8], aggregate_storage[9]);

  std::printf("{");
  std::printf("\"widget_found\":%d,", widget_entry.found);
  std::printf("\"traced_value\":%d,", traced_value);
  std::printf("\"count_value\":%d,", count_value);
  std::printf("\"count_property_found\":%d,", count_property.found);
  std::printf("\"tracer_conforms\":%d,", tracer_query.conforms);
  std::printf("\"aggregate\":{");
  std::printf("\"realized_class_count\":%llu,",
              static_cast<unsigned long long>(aggregate.realized_class_count));
  std::printf("\"reflectable_property_count\":%llu,",
              static_cast<unsigned long long>(
                  aggregate.reflectable_property_count));
  std::printf("\"attached_category_count\":%llu,",
              static_cast<unsigned long long>(
                  aggregate.attached_category_count));
  std::printf("\"protocol_conformance_edge_count\":%llu,",
              static_cast<unsigned long long>(
                  aggregate.protocol_conformance_edge_count));
  std::printf("\"method_cache_entry_count\":%llu,",
              static_cast<unsigned long long>(
                  aggregate.method_cache_entry_count));
  std::printf("\"last_class_query_found\":%d,",
              aggregate.last_class_query_found);
  std::printf("\"last_property_query_found\":%d,",
              aggregate.last_property_query_found);
  std::printf("\"last_property_query_inherited\":%d,",
              aggregate.last_property_query_inherited);
  std::printf("\"last_protocol_query_class_found\":%d,",
              aggregate.last_protocol_query_class_found);
  std::printf("\"last_protocol_query_protocol_found\":%d,",
              aggregate.last_protocol_query_protocol_found);
  std::printf("\"last_protocol_query_conforms\":%d,",
              aggregate.last_protocol_query_conforms);
  std::printf("\"last_queried_class_name\":");
  PrintJsonStringOrNull(aggregate.last_queried_class_name);
  std::printf(",\"last_resolved_class_name\":");
  PrintJsonStringOrNull(aggregate.last_resolved_class_name);
  std::printf(",\"last_resolved_class_owner_identity\":");
  PrintJsonStringOrNull(aggregate.last_resolved_class_owner_identity);
  std::printf(",\"last_queried_property_name\":");
  PrintJsonStringOrNull(aggregate.last_queried_property_name);
  std::printf(",\"last_resolved_property_class_name\":");
  PrintJsonStringOrNull(aggregate.last_resolved_property_class_name);
  std::printf(",\"last_resolved_property_owner_identity\":");
  PrintJsonStringOrNull(aggregate.last_resolved_property_owner_identity);
  std::printf(",\"last_queried_protocol_class_name\":");
  PrintJsonStringOrNull(aggregate.last_queried_protocol_class_name);
  std::printf(",\"last_queried_protocol_name\":");
  PrintJsonStringOrNull(aggregate.last_queried_protocol_name);
  std::printf(",\"last_matched_protocol_owner_identity\":");
  PrintJsonStringOrNull(aggregate.last_matched_protocol_owner_identity);
  std::printf(",\"last_matched_attachment_owner_identity\":");
  PrintJsonStringOrNull(aggregate.last_matched_attachment_owner_identity);
  std::printf("}}\n");
  return 0;
}
