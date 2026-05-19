#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_stabilizers.h"
#include "support/runtime_snapshot_json.h"
#include "support/typed_dispatch_helpers.h"

#include <cstdio>
#include <string>

namespace {

using objc3c::runtime::probe::PrintMethodCacheEntryBasic;
using objc3c::runtime::probe::PrintRegistrationStateBasic;
using objc3c::runtime::probe::PrintSelectorTableStateBasic;

using objc3c::runtime::probe::StabilizeMethodCacheEntry;
using objc3c::runtime::probe::StabilizeNullableCString;
using objc3c::runtime::probe::StabilizeRegistrationState;
using objc3c::runtime::probe::StabilizeSelectorTableState;

using objc3c::runtime::probe::PrintJsonStringOrNull;
using objc3c::runtime::probe::DispatchTypedBoolValue;
using objc3c::runtime::probe::DispatchTypedObjectReference;
using objc3c::runtime::probe::DispatchTypedStatus;


}  // namespace

int main() {
  objc3_runtime_registration_state_snapshot registration_state{};
  objc3_runtime_selector_lookup_table_state_snapshot selector_table_state{};
  objc3_runtime_method_cache_entry_snapshot count_entry{};
  objc3_runtime_method_cache_entry_snapshot set_count_entry{};

  std::string registration_module_storage;
  std::string registration_identity_storage;
  std::string selector_table_last_storage;
  std::string count_selector_storage;
  std::string count_class_storage;
  std::string count_owner_storage;
  std::string set_count_selector_storage;
  std::string set_count_class_storage;
  std::string set_count_owner_storage;

  (void)objc3_runtime_copy_registration_state_for_testing(&registration_state);
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &selector_table_state);
  StabilizeRegistrationState(registration_state, registration_module_storage,
                             registration_identity_storage);
  StabilizeSelectorTableState(selector_table_state, selector_table_last_storage);

  const int first_alloc = DispatchTypedObjectReference(1024, "alloc");
  const int second_alloc = DispatchTypedObjectReference(1024, "alloc");
  const int set_count_result =
      DispatchTypedStatus(first_alloc, "setCount:", 37);
  const int count_value_first =
      objc3_runtime_dispatch_i32(first_alloc, "count", 0, 0, 0, 0);
  const int count_value_second =
      objc3_runtime_dispatch_i32(second_alloc, "count", 0, 0, 0, 0);
  const int set_enabled_result =
      DispatchTypedStatus(first_alloc, "setEnabled:", 1);
  const int enabled_value_second =
      DispatchTypedBoolValue(second_alloc, "enabled");
  const int set_value_result =
      DispatchTypedStatus(first_alloc, "setValue:", 55);
  const int value_result_second =
      DispatchTypedObjectReference(second_alloc, "value");

  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      first_alloc, "count", &count_entry);
  StabilizeMethodCacheEntry(count_entry, count_selector_storage,
                            count_class_storage, count_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      first_alloc, "setCount:", &set_count_entry);
  StabilizeMethodCacheEntry(set_count_entry, set_count_selector_storage,
                            set_count_class_storage, set_count_owner_storage);

  std::printf("{");
  std::printf("\"first_alloc\":%d,", first_alloc);
  std::printf("\"second_alloc\":%d,", second_alloc);
  std::printf("\"set_count_result\":%d,", set_count_result);
  std::printf("\"count_value_first\":%d,", count_value_first);
  std::printf("\"count_value_second\":%d,", count_value_second);
  std::printf("\"set_enabled_result\":%d,", set_enabled_result);
  std::printf("\"enabled_value_second\":%d,", enabled_value_second);
  std::printf("\"set_value_result\":%d,", set_value_result);
  std::printf("\"value_result_second\":%d,", value_result_second);
  std::printf("\"registration_state\":");
  PrintRegistrationStateBasic(registration_state);
  std::printf(",\"selector_table_state\":");
  PrintSelectorTableStateBasic(selector_table_state);
  std::printf(",\"count_entry\":");
  PrintMethodCacheEntryBasic(count_entry);
  std::printf(",\"set_count_entry\":");
  PrintMethodCacheEntryBasic(set_count_entry);
  std::printf("}");
  return 0;
}
