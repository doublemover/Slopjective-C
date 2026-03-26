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

void StabilizeGraphState(
    objc3_runtime_realized_class_graph_state_snapshot &snapshot,
    std::string &class_storage, std::string &class_owner_storage,
    std::string &metaclass_owner_storage, std::string &category_owner_storage,
    std::string &category_name_storage) {
  StabilizeNullableCString(snapshot.last_realized_class_name, class_storage,
                           snapshot.last_realized_class_name);
  StabilizeNullableCString(snapshot.last_realized_class_owner_identity,
                           class_owner_storage,
                           snapshot.last_realized_class_owner_identity);
  StabilizeNullableCString(snapshot.last_realized_metaclass_owner_identity,
                           metaclass_owner_storage,
                           snapshot.last_realized_metaclass_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_owner_identity,
                           category_owner_storage,
                           snapshot.last_attached_category_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_name,
                           category_name_storage,
                           snapshot.last_attached_category_name);
}

void StabilizeRealizedEntry(
    objc3_runtime_realized_class_entry_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage,
    std::string &class_storage, std::string &class_owner_storage,
    std::string &metaclass_owner_storage,
    std::string &super_class_owner_storage,
    std::string &super_metaclass_owner_storage,
    std::string &category_owner_storage, std::string &category_name_storage) {
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
                           super_class_owner_storage,
                           snapshot.super_class_owner_identity);
  StabilizeNullableCString(snapshot.super_metaclass_owner_identity,
                           super_metaclass_owner_storage,
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
  StabilizeNullableCString(snapshot.selector, selector_storage,
                           snapshot.selector);
  StabilizeNullableCString(snapshot.resolved_class_name, class_storage,
                           snapshot.resolved_class_name);
  StabilizeNullableCString(snapshot.resolved_owner_identity, owner_storage,
                           snapshot.resolved_owner_identity);
}

void PrintGraphState(
    const objc3_runtime_realized_class_graph_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"realized_class_count\":%llu,",
              static_cast<unsigned long long>(snapshot.realized_class_count));
  std::printf("\"root_class_count\":%llu,",
              static_cast<unsigned long long>(snapshot.root_class_count));
  std::printf("\"metaclass_edge_count\":%llu,",
              static_cast<unsigned long long>(snapshot.metaclass_edge_count));
  std::printf("\"receiver_class_binding_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.receiver_class_binding_count));
  std::printf("\"attached_category_count\":%llu,",
              static_cast<unsigned long long>(snapshot.attached_category_count));
  std::printf("\"protocol_conformance_edge_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.protocol_conformance_edge_count));
  std::printf("\"last_realized_class_name\":");
  PrintJsonStringOrNull(snapshot.last_realized_class_name);
  std::printf(",\"last_realized_class_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_realized_class_owner_identity);
  std::printf(",\"last_realized_metaclass_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_realized_metaclass_owner_identity);
  std::printf(",\"last_attached_category_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_attached_category_owner_identity);
  std::printf(",\"last_attached_category_name\":");
  PrintJsonStringOrNull(snapshot.last_attached_category_name);
  std::printf("}");
}

void PrintRealizedEntry(
    const objc3_runtime_realized_class_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"base_identity\":%llu,",
              static_cast<unsigned long long>(snapshot.base_identity));
  std::printf("\"registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.registration_order_ordinal));
  std::printf("\"is_root_class\":%d,", snapshot.is_root_class);
  std::printf("\"implementation_backed\":%d,", snapshot.implementation_backed);
  std::printf("\"attached_category_count\":%llu,",
              static_cast<unsigned long long>(snapshot.attached_category_count));
  std::printf("\"direct_protocol_count\":%llu,",
              static_cast<unsigned long long>(snapshot.direct_protocol_count));
  std::printf("\"attached_protocol_count\":%llu,",
              static_cast<unsigned long long>(snapshot.attached_protocol_count));
  std::printf("\"module_name\":");
  PrintJsonStringOrNull(snapshot.module_name);
  std::printf(",\"translation_unit_identity_key\":");
  PrintJsonStringOrNull(snapshot.translation_unit_identity_key);
  std::printf(",\"class_name\":");
  PrintJsonStringOrNull(snapshot.class_name);
  std::printf(",\"class_owner_identity\":");
  PrintJsonStringOrNull(snapshot.class_owner_identity);
  std::printf(",\"metaclass_owner_identity\":");
  PrintJsonStringOrNull(snapshot.metaclass_owner_identity);
  std::printf(",\"super_class_owner_identity\":");
  PrintJsonStringOrNull(snapshot.super_class_owner_identity);
  std::printf(",\"super_metaclass_owner_identity\":");
  PrintJsonStringOrNull(snapshot.super_metaclass_owner_identity);
  std::printf(",\"last_attached_category_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_attached_category_owner_identity);
  std::printf(",\"last_attached_category_name\":");
  PrintJsonStringOrNull(snapshot.last_attached_category_name);
  std::printf("}");
}

void PrintConformanceQuery(
    const objc3_runtime_protocol_conformance_query_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"class_found\":%d,", snapshot.class_found);
  std::printf("\"protocol_found\":%d,", snapshot.protocol_found);
  std::printf("\"conforms\":%d,", snapshot.conforms);
  std::printf("\"visited_protocol_count\":%llu,",
              static_cast<unsigned long long>(snapshot.visited_protocol_count));
  std::printf("\"attached_category_count\":%llu,",
              static_cast<unsigned long long>(snapshot.attached_category_count));
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
  objc3_runtime_realized_class_graph_state_snapshot graph_state{};
  objc3_runtime_realized_class_entry_snapshot widget_entry{};
  objc3_runtime_protocol_conformance_query_snapshot worker_query{};
  objc3_runtime_protocol_conformance_query_snapshot tracer_query{};
  objc3_runtime_method_cache_state_snapshot method_state{};
  objc3_runtime_method_cache_entry_snapshot alloc_entry{};
  objc3_runtime_method_cache_entry_snapshot init_entry{};
  objc3_runtime_method_cache_entry_snapshot new_entry{};
  objc3_runtime_method_cache_entry_snapshot traced_entry{};
  objc3_runtime_method_cache_entry_snapshot inherited_entry{};
  objc3_runtime_method_cache_entry_snapshot class_entry{};

  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(&graph_state);
  (void)objc3_runtime_copy_realized_class_entry_for_testing("Widget", &widget_entry);

  const int widget_instance_receiver =
      widget_entry.found != 0 ? static_cast<int>(widget_entry.base_identity + 1U)
                              : 0;
  const int widget_class_receiver =
      widget_entry.found != 0 ? static_cast<int>(widget_entry.base_identity + 2U)
                              : 0;
  const objc3_runtime_selector_handle *alloc_selector =
      objc3_runtime_lookup_selector("alloc");
  const objc3_runtime_selector_handle *init_selector =
      objc3_runtime_lookup_selector("init");
  const objc3_runtime_selector_handle *new_selector =
      objc3_runtime_lookup_selector("new");
  const objc3_runtime_selector_handle *traced_selector =
      objc3_runtime_lookup_selector("tracedValue");
  const objc3_runtime_selector_handle *inherited_selector =
      objc3_runtime_lookup_selector("inheritedValue");
  const objc3_runtime_selector_handle *class_selector =
      objc3_runtime_lookup_selector("classValue");

  const int alloc_value =
      objc3_runtime_dispatch_i32(widget_class_receiver, "alloc", 0, 0, 0, 0);
  const int init_value =
      objc3_runtime_dispatch_i32(alloc_value, "init", 0, 0, 0, 0);
  const int new_value =
      objc3_runtime_dispatch_i32(widget_class_receiver, "new", 0, 0, 0, 0);
  const int traced_value =
      objc3_runtime_dispatch_i32(init_value, "tracedValue", 0, 0, 0, 0);
  const int inherited_value =
      objc3_runtime_dispatch_i32(init_value, "inheritedValue", 0, 0, 0, 0);
  const int class_value =
      objc3_runtime_dispatch_i32(widget_class_receiver, "classValue", 0, 0, 0, 0);

  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Widget", "Worker", &worker_query);
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Widget", "Tracer", &tracer_query);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&method_state);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(widget_class_receiver,
                                                          "alloc", &alloc_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(init_value, "init",
                                                          &init_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(widget_class_receiver,
                                                          "new", &new_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(init_value,
                                                          "tracedValue",
                                                          &traced_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(init_value,
                                                          "inheritedValue",
                                                          &inherited_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(widget_class_receiver,
                                                          "classValue",
                                                          &class_entry);

  std::string graph_class_storage;
  std::string graph_class_owner_storage;
  std::string graph_metaclass_owner_storage;
  std::string graph_category_owner_storage;
  std::string graph_category_name_storage;
  std::string widget_module_storage;
  std::string widget_identity_storage;
  std::string widget_class_storage;
  std::string widget_class_owner_storage;
  std::string widget_metaclass_owner_storage;
  std::string widget_super_class_owner_storage;
  std::string widget_super_metaclass_owner_storage;
  std::string widget_category_owner_storage;
  std::string widget_category_name_storage;
  std::string worker_class_storage;
  std::string worker_protocol_storage;
  std::string worker_protocol_owner_storage;
  std::string worker_attachment_owner_storage;
  std::string tracer_class_storage;
  std::string tracer_protocol_storage;
  std::string tracer_protocol_owner_storage;
  std::string tracer_attachment_owner_storage;
  std::string method_selector_storage;
  std::string method_class_storage;
  std::string method_owner_storage;
  std::string alloc_selector_storage;
  std::string alloc_class_storage;
  std::string alloc_owner_storage;
  std::string init_selector_storage;
  std::string init_class_storage;
  std::string init_owner_storage;
  std::string new_selector_storage;
  std::string new_class_storage;
  std::string new_owner_storage;
  std::string traced_selector_storage;
  std::string traced_class_storage;
  std::string traced_owner_storage;
  std::string inherited_selector_storage;
  std::string inherited_class_storage;
  std::string inherited_owner_storage;
  std::string class_selector_storage;
  std::string class_class_storage;
  std::string class_owner_storage;

  StabilizeGraphState(graph_state, graph_class_storage, graph_class_owner_storage,
                      graph_metaclass_owner_storage,
                      graph_category_owner_storage, graph_category_name_storage);
  StabilizeRealizedEntry(widget_entry, widget_module_storage, widget_identity_storage,
                         widget_class_storage, widget_class_owner_storage,
                         widget_metaclass_owner_storage,
                         widget_super_class_owner_storage,
                         widget_super_metaclass_owner_storage,
                         widget_category_owner_storage,
                         widget_category_name_storage);
  StabilizeConformanceQuery(worker_query, worker_class_storage,
                            worker_protocol_storage,
                            worker_protocol_owner_storage,
                            worker_attachment_owner_storage);
  StabilizeConformanceQuery(tracer_query, tracer_class_storage,
                            tracer_protocol_storage,
                            tracer_protocol_owner_storage,
                            tracer_attachment_owner_storage);
  StabilizeMethodCacheState(method_state, method_selector_storage,
                            method_class_storage, method_owner_storage);
  StabilizeMethodCacheEntry(alloc_entry, alloc_selector_storage,
                            alloc_class_storage, alloc_owner_storage);
  StabilizeMethodCacheEntry(init_entry, init_selector_storage,
                            init_class_storage, init_owner_storage);
  StabilizeMethodCacheEntry(new_entry, new_selector_storage,
                            new_class_storage, new_owner_storage);
  StabilizeMethodCacheEntry(traced_entry, traced_selector_storage,
                            traced_class_storage, traced_owner_storage);
  StabilizeMethodCacheEntry(inherited_entry, inherited_selector_storage,
                            inherited_class_storage, inherited_owner_storage);
  StabilizeMethodCacheEntry(class_entry, class_selector_storage,
                            class_class_storage, class_owner_storage);

  std::printf("{");
  std::printf("\"alloc_value\":%d,", alloc_value);
  std::printf("\"init_value\":%d,", init_value);
  std::printf("\"new_value\":%d,", new_value);
  std::printf("\"traced_value\":%d,", traced_value);
  std::printf("\"inherited_value\":%d,", inherited_value);
  std::printf("\"class_value\":%d,", class_value);
  std::printf("\"widget_instance_receiver\":%d,", widget_instance_receiver);
  std::printf("\"widget_class_receiver\":%d,", widget_class_receiver);
  std::printf("\"selector_handles\":{");
  std::printf("\"alloc\":%llu,",
              static_cast<unsigned long long>(alloc_selector != nullptr ? alloc_selector->stable_id : 0));
  std::printf("\"init\":%llu,",
              static_cast<unsigned long long>(init_selector != nullptr ? init_selector->stable_id : 0));
  std::printf("\"new\":%llu,",
              static_cast<unsigned long long>(new_selector != nullptr ? new_selector->stable_id : 0));
  std::printf("\"tracedValue\":%llu,",
              static_cast<unsigned long long>(traced_selector != nullptr ? traced_selector->stable_id : 0));
  std::printf("\"inheritedValue\":%llu,",
              static_cast<unsigned long long>(inherited_selector != nullptr ? inherited_selector->stable_id : 0));
  std::printf("\"classValue\":%llu},",
              static_cast<unsigned long long>(class_selector != nullptr ? class_selector->stable_id : 0));
  std::printf("\"graph_state\":");
  PrintGraphState(graph_state);
  std::printf(",\"widget_entry\":");
  PrintRealizedEntry(widget_entry);
  std::printf(",\"worker_query\":");
  PrintConformanceQuery(worker_query);
  std::printf(",\"tracer_query\":");
  PrintConformanceQuery(tracer_query);
  std::printf(",\"method_state\":");
  PrintMethodCacheState(method_state);
  std::printf(",\"alloc_entry\":");
  PrintMethodCacheEntry(alloc_entry);
  std::printf(",\"init_entry\":");
  PrintMethodCacheEntry(init_entry);
  std::printf(",\"new_entry\":");
  PrintMethodCacheEntry(new_entry);
  std::printf(",\"traced_entry\":");
  PrintMethodCacheEntry(traced_entry);
  std::printf(",\"inherited_entry\":");
  PrintMethodCacheEntry(inherited_entry);
  std::printf(",\"class_entry\":");
  PrintMethodCacheEntry(class_entry);
  std::printf("}\n");
  return 0;
}
