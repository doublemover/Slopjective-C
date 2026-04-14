#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_json.h"
#include "support/runtime_snapshot_stabilizers.h"

#include <cstdio>
#include <string>

namespace {

using objc3c::runtime::probe::PrintMethodCacheEntryBasic;
using objc3c::runtime::probe::PrintRealizedClassEntryAllocation;
using objc3c::runtime::probe::PrintRealizedGraphStateAllocation;
using objc3c::runtime::probe::PrintRegistrationStateBasic;
using objc3c::runtime::probe::PrintSelectorTableStateBasic;

using objc3c::runtime::probe::StabilizeMethodCacheEntry;
using objc3c::runtime::probe::StabilizeNullableCString;
using objc3c::runtime::probe::StabilizeRealizedClassEntry;
using objc3c::runtime::probe::StabilizeRealizedGraphState;
using objc3c::runtime::probe::StabilizeRegistrationState;
using objc3c::runtime::probe::StabilizeSelectorTableState;

using objc3c::runtime::probe::PrintJsonStringOrNull;

} // namespace

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
  StabilizeSelectorTableState(selector_table_state,
                              selector_table_last_storage);

  const int first_alloc = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  const int second_alloc =
      objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);

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

  (void)objc3_runtime_copy_method_cache_entry_for_testing(first_alloc, "count",
                                                          &count_entry);
  StabilizeMethodCacheEntry(count_entry, count_selector_storage,
                            count_class_storage, count_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      first_alloc, "setCount:", &set_count_entry);
  StabilizeMethodCacheEntry(set_count_entry, set_count_selector_storage,
                            set_count_class_storage, set_count_owner_storage);

  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(&graph_state);
  StabilizeRealizedGraphState(
      graph_state, graph_class_storage, graph_owner_storage,
      graph_metaclass_storage, graph_category_owner_storage,
      graph_category_name_storage, graph_allocated_class_storage);
  (void)objc3_runtime_copy_realized_class_entry_for_testing("Widget",
                                                            &widget_entry);
  StabilizeRealizedClassEntry(
      widget_entry, widget_module_storage, widget_identity_storage,
      widget_class_storage, widget_class_owner_storage,
      widget_metaclass_owner_storage, widget_super_class_storage,
      widget_super_metaclass_storage, widget_category_owner_storage,
      widget_category_name_storage);

  std::printf("{");
  std::printf("\"first_alloc\":%d,", first_alloc);
  std::printf("\"second_alloc\":%d,", second_alloc);
  std::printf("\"set_count_first\":%d,", set_count_first);
  std::printf("\"count_value_first\":%d,", count_value_first);
  std::printf("\"count_value_second_before\":%d,", count_value_second_before);
  std::printf("\"set_enabled_first\":%d,", set_enabled_first);
  std::printf("\"enabled_value_first\":%d,", enabled_value_first);
  std::printf("\"enabled_value_second\":%d,", enabled_value_second);
  std::printf("\"set_value_first\":%d,", set_value_first);
  std::printf("\"value_result_first\":%d,", value_result_first);
  std::printf("\"value_result_second_before\":%d,", value_result_second_before);
  std::printf("\"set_count_second\":%d,", set_count_second);
  std::printf("\"count_value_first_after_second\":%d,",
              count_value_first_after_second);
  std::printf("\"count_value_second_after\":%d,", count_value_second_after);
  std::printf("\"set_value_second\":%d,", set_value_second);
  std::printf("\"value_result_first_after_second\":%d,",
              value_result_first_after_second);
  std::printf("\"value_result_second_after\":%d,", value_result_second_after);
  std::printf("\"registration_state\":");
  PrintRegistrationStateBasic(registration_state);
  std::printf(",\"selector_table_state\":");
  PrintSelectorTableStateBasic(selector_table_state);
  std::printf(",\"graph_state\":");
  PrintRealizedGraphStateAllocation(graph_state);
  std::printf(",\"widget_entry\":");
  PrintRealizedClassEntryAllocation(widget_entry);
  std::printf(",\"count_entry\":");
  PrintMethodCacheEntryBasic(count_entry);
  std::printf(",\"set_count_entry\":");
  PrintMethodCacheEntryBasic(set_count_entry);
  std::printf("}");
  return 0;
}
