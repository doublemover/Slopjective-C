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
using objc3c::runtime::probe::PrintMethodCacheEntryRuntimeCanonical;
using objc3c::runtime::probe::PrintMethodCacheStateFull;
using objc3c::runtime::probe::PrintRealizedEntryProtocolCategory;
using objc3c::runtime::probe::PrintSelectorEntryBasic;
using objc3c::runtime::probe::PrintSelectorTableStateRuntimeCanonical;

using objc3c::runtime::probe::StabilizeConformanceQuery;
using objc3c::runtime::probe::StabilizeGraphState;
using objc3c::runtime::probe::StabilizeMethodCacheEntry;
using objc3c::runtime::probe::StabilizeMethodCacheState;
using objc3c::runtime::probe::StabilizeNullableCString;
using objc3c::runtime::probe::StabilizeRealizedEntry;
using objc3c::runtime::probe::StabilizeSelectorEntry;
using objc3c::runtime::probe::StabilizeSelectorTableState;

using objc3c::runtime::probe::PrintJsonStringOrNull;

} // namespace

int main() {
  objc3_runtime_realized_class_graph_state_snapshot graph_state{};
  objc3_runtime_realized_class_entry_snapshot widget_entry{};
  objc3_runtime_protocol_conformance_query_snapshot worker_query{};
  objc3_runtime_protocol_conformance_query_snapshot tracer_query{};
  objc3_runtime_method_cache_state_snapshot method_state{};
  objc3_runtime_method_cache_state_snapshot inherited_state{};
  objc3_runtime_method_cache_state_snapshot traced_state{};
  objc3_runtime_method_cache_state_snapshot class_state{};
  objc3_runtime_method_cache_state_snapshot ignored_state{};
  objc3_runtime_method_cache_state_snapshot ignored_cached_state{};
  objc3_runtime_method_cache_entry_snapshot alloc_entry{};
  objc3_runtime_method_cache_entry_snapshot init_entry{};
  objc3_runtime_method_cache_entry_snapshot new_entry{};
  objc3_runtime_method_cache_entry_snapshot traced_entry{};
  objc3_runtime_method_cache_entry_snapshot inherited_entry{};
  objc3_runtime_method_cache_entry_snapshot class_entry{};
  objc3_runtime_method_cache_entry_snapshot ignored_entry{};
  objc3_runtime_selector_lookup_table_state_snapshot selector_table_state{};
  objc3_runtime_selector_lookup_entry_snapshot traced_selector_entry{};
  objc3_runtime_selector_lookup_entry_snapshot inherited_selector_entry{};
  objc3_runtime_selector_lookup_entry_snapshot class_selector_entry{};
  objc3_runtime_selector_lookup_entry_snapshot ignored_selector_entry{};

  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(&graph_state);
  (void)objc3_runtime_copy_realized_class_entry_for_testing("Widget",
                                                            &widget_entry);

  const int widget_instance_receiver =
      widget_entry.found != 0
          ? static_cast<int>(widget_entry.base_identity + 1U)
          : 0;
  const int widget_class_receiver =
      widget_entry.found != 0
          ? static_cast<int>(widget_entry.base_identity + 2U)
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
  const int inherited_value =
      objc3_runtime_dispatch_i32(init_value, "inheritedValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&inherited_state);
  const int traced_value =
      objc3_runtime_dispatch_i32(init_value, "tracedValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&traced_state);
  const int class_value = objc3_runtime_dispatch_i32(widget_class_receiver,
                                                     "classValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&class_state);
  const objc3_runtime_dispatch_i32_result ignored_result =
      objc3_runtime_dispatch_i32_checked(init_value, "ignoredValue", 0, 0, 0, 0);
  const int ignored_value = ignored_result.value;
  (void)objc3_runtime_copy_method_cache_state_for_testing(&ignored_state);
  const objc3_runtime_dispatch_i32_result ignored_cached_result =
      objc3_runtime_dispatch_i32_checked(init_value, "ignoredValue", 0, 0, 0, 0);
  const int ignored_cached_value = ignored_cached_result.value;
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &ignored_cached_state);
  const int ignored_expected =
      ExpectedStrictDispatchErrorValue(init_value, "ignoredValue", 0, 0, 0, 0);

  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Widget", "Worker", &worker_query);
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Widget", "Tracer", &tracer_query);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&method_state);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      widget_class_receiver, "alloc", &alloc_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(init_value, "init",
                                                          &init_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(widget_class_receiver,
                                                          "new", &new_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      init_value, "tracedValue", &traced_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      init_value, "inheritedValue", &inherited_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      widget_class_receiver, "classValue", &class_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      init_value, "ignoredValue", &ignored_entry);
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &selector_table_state);
  (void)objc3_runtime_copy_selector_lookup_entry_for_testing(
      "tracedValue", &traced_selector_entry);
  (void)objc3_runtime_copy_selector_lookup_entry_for_testing(
      "inheritedValue", &inherited_selector_entry);
  (void)objc3_runtime_copy_selector_lookup_entry_for_testing(
      "classValue", &class_selector_entry);
  (void)objc3_runtime_copy_selector_lookup_entry_for_testing(
      "ignoredValue", &ignored_selector_entry);

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
  std::string inherited_state_selector_storage;
  std::string inherited_state_class_storage;
  std::string inherited_state_owner_storage;
  std::string traced_state_selector_storage;
  std::string traced_state_class_storage;
  std::string traced_state_owner_storage;
  std::string class_state_selector_storage;
  std::string class_state_class_storage;
  std::string class_state_owner_storage;
  std::string ignored_state_selector_storage;
  std::string ignored_state_class_storage;
  std::string ignored_state_owner_storage;
  std::string ignored_cached_state_selector_storage;
  std::string ignored_cached_state_class_storage;
  std::string ignored_cached_state_owner_storage;
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
  std::string ignored_selector_storage;
  std::string ignored_class_storage;
  std::string ignored_owner_storage;
  std::string selector_table_last_storage;
  std::string traced_selector_canonical_storage;
  std::string inherited_selector_canonical_storage;
  std::string class_selector_canonical_storage;
  std::string ignored_selector_canonical_storage;

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
  StabilizeConformanceQuery(
      worker_query, worker_class_storage, worker_protocol_storage,
      worker_protocol_owner_storage, worker_attachment_owner_storage);
  StabilizeConformanceQuery(
      tracer_query, tracer_class_storage, tracer_protocol_storage,
      tracer_protocol_owner_storage, tracer_attachment_owner_storage);
  StabilizeMethodCacheState(method_state, method_selector_storage,
                            method_class_storage, method_owner_storage);
  StabilizeMethodCacheState(inherited_state, inherited_state_selector_storage,
                            inherited_state_class_storage,
                            inherited_state_owner_storage);
  StabilizeMethodCacheState(traced_state, traced_state_selector_storage,
                            traced_state_class_storage,
                            traced_state_owner_storage);
  StabilizeMethodCacheState(class_state, class_state_selector_storage,
                            class_state_class_storage,
                            class_state_owner_storage);
  StabilizeMethodCacheState(ignored_state, ignored_state_selector_storage,
                            ignored_state_class_storage,
                            ignored_state_owner_storage);
  StabilizeMethodCacheState(
      ignored_cached_state, ignored_cached_state_selector_storage,
      ignored_cached_state_class_storage, ignored_cached_state_owner_storage);
  StabilizeMethodCacheEntry(alloc_entry, alloc_selector_storage,
                            alloc_class_storage, alloc_owner_storage);
  StabilizeMethodCacheEntry(init_entry, init_selector_storage,
                            init_class_storage, init_owner_storage);
  StabilizeMethodCacheEntry(new_entry, new_selector_storage, new_class_storage,
                            new_owner_storage);
  StabilizeMethodCacheEntry(traced_entry, traced_selector_storage,
                            traced_class_storage, traced_owner_storage);
  StabilizeMethodCacheEntry(inherited_entry, inherited_selector_storage,
                            inherited_class_storage, inherited_owner_storage);
  StabilizeMethodCacheEntry(class_entry, class_selector_storage,
                            class_class_storage, class_owner_storage);
  StabilizeMethodCacheEntry(ignored_entry, ignored_selector_storage,
                            ignored_class_storage, ignored_owner_storage);
  StabilizeSelectorTableState(selector_table_state,
                              selector_table_last_storage);
  StabilizeSelectorEntry(traced_selector_entry,
                         traced_selector_canonical_storage);
  StabilizeSelectorEntry(inherited_selector_entry,
                         inherited_selector_canonical_storage);
  StabilizeSelectorEntry(class_selector_entry,
                         class_selector_canonical_storage);
  StabilizeSelectorEntry(ignored_selector_entry,
                         ignored_selector_canonical_storage);

  std::printf("{");
  std::printf("\"alloc_value\":%d,", alloc_value);
  std::printf("\"init_value\":%d,", init_value);
  std::printf("\"new_value\":%d,", new_value);
  std::printf("\"traced_value\":%d,", traced_value);
  std::printf("\"inherited_value\":%d,", inherited_value);
  std::printf("\"class_value\":%d,", class_value);
  std::printf("\"ignored_value\":%d,", ignored_value);
  std::printf("\"ignored_cached_value\":%d,", ignored_cached_value);
  std::printf("\"ignored_expected\":%d,", ignored_expected);
  std::printf("\"widget_instance_receiver\":%d,", widget_instance_receiver);
  std::printf("\"widget_class_receiver\":%d,", widget_class_receiver);
  std::printf("\"selector_handles\":{");
  std::printf("\"alloc\":%llu,",
              static_cast<unsigned long long>(
                  alloc_selector != nullptr ? alloc_selector->stable_id : 0));
  std::printf("\"init\":%llu,",
              static_cast<unsigned long long>(
                  init_selector != nullptr ? init_selector->stable_id : 0));
  std::printf("\"new\":%llu,",
              static_cast<unsigned long long>(
                  new_selector != nullptr ? new_selector->stable_id : 0));
  std::printf("\"tracedValue\":%llu,",
              static_cast<unsigned long long>(
                  traced_selector != nullptr ? traced_selector->stable_id : 0));
  std::printf(
      "\"inheritedValue\":%llu,",
      static_cast<unsigned long long>(
          inherited_selector != nullptr ? inherited_selector->stable_id : 0));
  std::printf("\"classValue\":%llu},",
              static_cast<unsigned long long>(
                  class_selector != nullptr ? class_selector->stable_id : 0));
  std::printf("\"graph_state\":");
  PrintGraphStateProtocolCategory(graph_state);
  std::printf(",\"widget_entry\":");
  PrintRealizedEntryProtocolCategory(widget_entry);
  std::printf(",\"worker_query\":");
  PrintConformanceQueryProtocolCategory(worker_query);
  std::printf(",\"tracer_query\":");
  PrintConformanceQueryProtocolCategory(tracer_query);
  std::printf(",\"method_state\":");
  PrintMethodCacheStateFull(method_state);
  std::printf(",\"inherited_state\":");
  PrintMethodCacheStateFull(inherited_state);
  std::printf(",\"traced_state\":");
  PrintMethodCacheStateFull(traced_state);
  std::printf(",\"class_state\":");
  PrintMethodCacheStateFull(class_state);
  std::printf(",\"ignored_state\":");
  PrintMethodCacheStateFull(ignored_state);
  std::printf(",\"ignored_cached_state\":");
  PrintMethodCacheStateFull(ignored_cached_state);
  std::printf(",\"alloc_entry\":");
  PrintMethodCacheEntryRuntimeCanonical(alloc_entry);
  std::printf(",\"init_entry\":");
  PrintMethodCacheEntryRuntimeCanonical(init_entry);
  std::printf(",\"new_entry\":");
  PrintMethodCacheEntryRuntimeCanonical(new_entry);
  std::printf(",\"traced_entry\":");
  PrintMethodCacheEntryRuntimeCanonical(traced_entry);
  std::printf(",\"inherited_entry\":");
  PrintMethodCacheEntryRuntimeCanonical(inherited_entry);
  std::printf(",\"class_entry\":");
  PrintMethodCacheEntryRuntimeCanonical(class_entry);
  std::printf(",\"ignored_entry\":");
  PrintMethodCacheEntryRuntimeCanonical(ignored_entry);
  std::printf(",\"selector_table_state\":");
  PrintSelectorTableStateRuntimeCanonical(selector_table_state);
  std::printf(",\"traced_selector_entry\":");
  PrintSelectorEntryBasic(traced_selector_entry);
  std::printf(",\"inherited_selector_entry\":");
  PrintSelectorEntryBasic(inherited_selector_entry);
  std::printf(",\"class_selector_entry\":");
  PrintSelectorEntryBasic(class_selector_entry);
  std::printf(",\"ignored_selector_entry\":");
  PrintSelectorEntryBasic(ignored_selector_entry);
  std::printf("}\n");
  return 0;
}
