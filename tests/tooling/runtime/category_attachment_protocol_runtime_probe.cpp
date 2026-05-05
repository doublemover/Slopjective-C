#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/dispatch_expectations.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_json.h"
#include "support/runtime_snapshot_stabilizers.h"

#include <cstdio>
#include <string>

namespace {

using objc3c::runtime::probe::ExpectedStrictDispatchErrorValue;

using objc3c::runtime::probe::PrintConformanceQueryProtocolCategory;
using objc3c::runtime::probe::PrintGraphStateProtocolCategory;
using objc3c::runtime::probe::PrintMethodCacheStateCategoryAttachment;
using objc3c::runtime::probe::PrintRealizedEntryProtocolCategory;

using objc3c::runtime::probe::StabilizeConformanceQuery;
using objc3c::runtime::probe::StabilizeGraphState;
using objc3c::runtime::probe::StabilizeMethodCacheState;
using objc3c::runtime::probe::StabilizeNullableCString;
using objc3c::runtime::probe::StabilizeRealizedEntry;

using objc3c::runtime::probe::PrintJsonStringOrNull;

} // namespace

int main() {
  const int category_value =
      objc3_runtime_dispatch_i32(1042, "tracedValue", 0, 0, 0, 0);
  const int class_value =
      objc3_runtime_dispatch_i32(1043, "classValue", 0, 0, 0, 0);
  const int protocol_fallback =
      objc3_runtime_dispatch_i32(1042, "ignoredValue", 0, 0, 0, 0);
  const int protocol_fallback_expected =
      ExpectedStrictDispatchErrorValue(1042, "ignoredValue", 0, 0, 0, 0);

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
  (void)objc3_runtime_copy_realized_class_entry_for_testing("Widget",
                                                            &widget_entry);
  (void)objc3_runtime_copy_realized_class_entry_for_testing("Base",
                                                            &base_entry);
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Widget", "Worker", &worker_query);
  StabilizeConformanceQuery(
      worker_query, worker_class_storage, worker_protocol_storage,
      worker_protocol_owner_storage, worker_attachment_owner_storage);
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Widget", "Tracer", &tracer_query);
  StabilizeConformanceQuery(
      tracer_query, tracer_class_storage, tracer_protocol_storage,
      tracer_protocol_owner_storage, tracer_attachment_owner_storage);
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

  StabilizeGraphState(graph_state, graph_class_storage,
                      graph_class_owner_storage, graph_metaclass_owner_storage,
                      graph_category_owner_storage,
                      graph_category_name_storage);
  StabilizeRealizedEntry(
      widget_entry, widget_module_storage, widget_identity_storage,
      widget_class_storage, widget_class_owner_storage,
      widget_metaclass_owner_storage, widget_super_class_owner_storage,
      widget_super_metaclass_owner_storage, widget_category_owner_storage,
      widget_category_name_storage);
  StabilizeRealizedEntry(
      base_entry, base_module_storage, base_identity_storage,
      base_class_storage, base_class_owner_storage,
      base_metaclass_owner_storage, base_super_class_owner_storage,
      base_super_metaclass_owner_storage, base_category_owner_storage,
      base_category_name_storage);
  StabilizeMethodCacheState(method_state, method_selector_storage,
                            method_class_storage, method_owner_storage);

  std::printf("{");
  std::printf("\"category_value\":%d,", category_value);
  std::printf("\"class_value\":%d,", class_value);
  std::printf("\"protocol_fallback\":%d,", protocol_fallback);
  std::printf("\"protocol_fallback_expected\":%d,", protocol_fallback_expected);
  std::printf("\"graph_state\":");
  PrintGraphStateProtocolCategory(graph_state);
  std::printf(",\"widget_entry\":");
  PrintRealizedEntryProtocolCategory(widget_entry);
  std::printf(",\"base_entry\":");
  PrintRealizedEntryProtocolCategory(base_entry);
  std::printf(",\"worker_query\":");
  PrintConformanceQueryProtocolCategory(worker_query);
  std::printf(",\"tracer_query\":");
  PrintConformanceQueryProtocolCategory(tracer_query);
  std::printf(",\"base_worker_query\":");
  PrintConformanceQueryProtocolCategory(base_worker_query);
  std::printf(",\"method_state\":");
  PrintMethodCacheStateCategoryAttachment(method_state);
  std::printf("}\n");
  return 0;
}
