#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_stabilizers.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>
#include <string>

namespace {

using objc3c::runtime::probe::PrintMethodCacheStateCategoryAttachment;

using objc3c::runtime::probe::StabilizeConformanceQuery;
using objc3c::runtime::probe::StabilizeGraphState;
using objc3c::runtime::probe::StabilizeMethodCacheState;
using objc3c::runtime::probe::StabilizeNullableCString;
using objc3c::runtime::probe::StabilizeRealizedEntry;

using objc3c::runtime::probe::PrintJsonStringOrNull;

constexpr long long kDispatchModulus = 2147483629LL;


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
              static_cast<unsigned long long>(snapshot.receiver_class_binding_count));
  std::printf("\"attached_category_count\":%llu,",
              static_cast<unsigned long long>(snapshot.attached_category_count));
  std::printf("\"protocol_conformance_edge_count\":%llu,",
              static_cast<unsigned long long>(snapshot.protocol_conformance_edge_count));
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
              static_cast<unsigned long long>(snapshot.registration_order_ordinal));
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


}  // namespace

int main() {
  const int category_value =
      objc3_runtime_dispatch_i32(1042, "tracedValue", 0, 0, 0, 0);
  const int class_value =
      objc3_runtime_dispatch_i32(1043, "classValue", 0, 0, 0, 0);
  const int protocol_fallback =
      objc3_runtime_dispatch_i32(1042, "ignoredValue", 0, 0, 0, 0);
  const int protocol_fallback_expected =
      ComputeFallbackDispatch(1042, "ignoredValue", 0, 0, 0, 0);

  objc3_runtime_realized_class_graph_state_snapshot graph_state{};
  objc3_runtime_realized_class_entry_snapshot widget_entry{};
  objc3_runtime_realized_class_entry_snapshot base_entry{};
  objc3_runtime_protocol_conformance_query_snapshot worker_query{};
  objc3_runtime_protocol_conformance_query_snapshot tracer_query{};
  objc3_runtime_protocol_conformance_query_snapshot base_worker_query{};
  objc3_runtime_method_cache_state_snapshot method_state{};
  std::string worker_class_storage;
  std::string worker_protocol_storage;
  std::string worker_protocol_owner_storage;
  std::string worker_attachment_owner_storage;
  std::string tracer_class_storage;
  std::string tracer_protocol_storage;
  std::string tracer_protocol_owner_storage;
  std::string tracer_attachment_owner_storage;
  std::string base_worker_class_storage;
  std::string base_worker_protocol_storage;
  std::string base_worker_protocol_owner_storage;
  std::string base_worker_attachment_owner_storage;

  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(&graph_state);
  (void)objc3_runtime_copy_realized_class_entry_for_testing("Widget", &widget_entry);
  (void)objc3_runtime_copy_realized_class_entry_for_testing("Base", &base_entry);
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Widget", "Worker", &worker_query);
  StabilizeConformanceQuery(worker_query, worker_class_storage,
                            worker_protocol_storage,
                            worker_protocol_owner_storage,
                            worker_attachment_owner_storage);
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Widget", "Tracer", &tracer_query);
  StabilizeConformanceQuery(tracer_query, tracer_class_storage,
                            tracer_protocol_storage,
                            tracer_protocol_owner_storage,
                            tracer_attachment_owner_storage);
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Base", "Worker", &base_worker_query);
  StabilizeConformanceQuery(base_worker_query, base_worker_class_storage,
                            base_worker_protocol_storage,
                            base_worker_protocol_owner_storage,
                            base_worker_attachment_owner_storage);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&method_state);

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
  std::string base_module_storage;
  std::string base_identity_storage;
  std::string base_class_storage;
  std::string base_class_owner_storage;
  std::string base_metaclass_owner_storage;
  std::string base_super_class_owner_storage;
  std::string base_super_metaclass_owner_storage;
  std::string base_category_owner_storage;
  std::string base_category_name_storage;
  std::string method_selector_storage;
  std::string method_class_storage;
  std::string method_owner_storage;

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
  StabilizeRealizedEntry(base_entry, base_module_storage, base_identity_storage,
                         base_class_storage, base_class_owner_storage,
                         base_metaclass_owner_storage,
                         base_super_class_owner_storage,
                         base_super_metaclass_owner_storage,
                         base_category_owner_storage,
                         base_category_name_storage);
  StabilizeMethodCacheState(method_state, method_selector_storage,
                            method_class_storage, method_owner_storage);

  std::printf("{");
  std::printf("\"category_value\":%d,", category_value);
  std::printf("\"class_value\":%d,", class_value);
  std::printf("\"protocol_fallback\":%d,", protocol_fallback);
  std::printf("\"protocol_fallback_expected\":%d,",
              protocol_fallback_expected);
  std::printf("\"graph_state\":");
  PrintGraphState(graph_state);
  std::printf(",\"widget_entry\":");
  PrintRealizedEntry(widget_entry);
  std::printf(",\"base_entry\":");
  PrintRealizedEntry(base_entry);
  std::printf(",\"worker_query\":");
  PrintConformanceQuery(worker_query);
  std::printf(",\"tracer_query\":");
  PrintConformanceQuery(tracer_query);
  std::printf(",\"base_worker_query\":");
  PrintConformanceQuery(base_worker_query);
  std::printf(",\"method_state\":");
  PrintMethodCacheStateCategoryAttachment(method_state);
  std::printf("}\n");
  return 0;
}
