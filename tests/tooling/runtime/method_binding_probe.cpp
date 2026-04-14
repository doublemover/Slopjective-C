#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_stabilizers.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>
#include <string>

namespace {

using objc3c::runtime::probe::PrintMethodCacheEntryBasic;
using objc3c::runtime::probe::PrintMethodCacheStateMethodBinding;
using objc3c::runtime::probe::PrintRegistrationStateBasic;
using objc3c::runtime::probe::PrintSelectorTableStateBasic;

using objc3c::runtime::probe::StabilizeMethodCacheEntry;
using objc3c::runtime::probe::StabilizeMethodCacheState;
using objc3c::runtime::probe::StabilizeNullableCString;
using objc3c::runtime::probe::StabilizeRegistrationState;
using objc3c::runtime::probe::StabilizeSelectorTableState;

using objc3c::runtime::probe::PrintJsonStringOrNull;


}  // namespace

int main() {
  objc3_runtime_registration_state_snapshot registration_state{};
  objc3_runtime_selector_lookup_table_state_snapshot selector_table_state{};
  objc3_runtime_method_cache_state_snapshot instance_first_state{};
  objc3_runtime_method_cache_state_snapshot instance_second_state{};
  objc3_runtime_method_cache_state_snapshot class_state{};
  objc3_runtime_method_cache_state_snapshot known_class_state{};
  objc3_runtime_method_cache_state_snapshot category_state{};
  objc3_runtime_method_cache_entry_snapshot instance_entry{};
  objc3_runtime_method_cache_entry_snapshot class_entry{};
  objc3_runtime_method_cache_entry_snapshot category_entry{};

  std::string registration_module_storage;
  std::string registration_identity_storage;
  std::string selector_table_last_storage;
  std::string instance_first_selector_storage;
  std::string instance_first_class_storage;
  std::string instance_first_owner_storage;
  std::string instance_second_selector_storage;
  std::string instance_second_class_storage;
  std::string instance_second_owner_storage;
  std::string class_selector_storage;
  std::string class_class_storage;
  std::string class_owner_storage;
  std::string known_class_selector_storage;
  std::string known_class_class_storage;
  std::string known_class_owner_storage;
  std::string category_selector_storage;
  std::string category_class_storage;
  std::string category_owner_storage;
  std::string instance_entry_selector_storage;
  std::string instance_entry_class_storage;
  std::string instance_entry_owner_storage;
  std::string class_entry_selector_storage;
  std::string class_entry_class_storage;
  std::string class_entry_owner_storage;
  std::string category_entry_selector_storage;
  std::string category_entry_class_storage;
  std::string category_entry_owner_storage;

  (void)objc3_runtime_copy_registration_state_for_testing(&registration_state);
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &selector_table_state);
  StabilizeRegistrationState(registration_state, registration_module_storage,
                             registration_identity_storage);
  StabilizeSelectorTableState(selector_table_state, selector_table_last_storage);

  const int instance_first =
      objc3_runtime_dispatch_i32(1025, "value:extra:", 7, 8, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&instance_first_state);
  StabilizeMethodCacheState(instance_first_state, instance_first_selector_storage,
                            instance_first_class_storage,
                            instance_first_owner_storage);

  const int instance_second =
      objc3_runtime_dispatch_i32(1025, "value:extra:", 7, 8, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&instance_second_state);
  StabilizeMethodCacheState(instance_second_state,
                            instance_second_selector_storage,
                            instance_second_class_storage,
                            instance_second_owner_storage);

  const int class_value =
      objc3_runtime_dispatch_i32(1026, "classValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&class_state);
  StabilizeMethodCacheState(class_state, class_selector_storage,
                            class_class_storage, class_owner_storage);

  const int known_class_value =
      objc3_runtime_dispatch_i32(1024, "classValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&known_class_state);
  StabilizeMethodCacheState(known_class_state, known_class_selector_storage,
                            known_class_class_storage,
                            known_class_owner_storage);

  const int category_value =
      objc3_runtime_dispatch_i32(1025, "tracedValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&category_state);
  StabilizeMethodCacheState(category_state, category_selector_storage,
                            category_class_storage, category_owner_storage);

  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      1025, "value:extra:", &instance_entry);
  StabilizeMethodCacheEntry(instance_entry, instance_entry_selector_storage,
                            instance_entry_class_storage,
                            instance_entry_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      1024, "classValue", &class_entry);
  StabilizeMethodCacheEntry(class_entry, class_entry_selector_storage,
                            class_entry_class_storage,
                            class_entry_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      1025, "tracedValue", &category_entry);
  StabilizeMethodCacheEntry(category_entry, category_entry_selector_storage,
                            category_entry_class_storage,
                            category_entry_owner_storage);

  std::printf("{");
  std::printf("\"instance_first\":%d,", instance_first);
  std::printf("\"instance_second\":%d,", instance_second);
  std::printf("\"class_value\":%d,", class_value);
  std::printf("\"known_class_value\":%d,", known_class_value);
  std::printf("\"category_value\":%d,", category_value);
  std::printf("\"registration_state\":");
  PrintRegistrationStateBasic(registration_state);
  std::printf(",\"selector_table_state\":");
  PrintSelectorTableStateBasic(selector_table_state);
  std::printf(",\"instance_first_state\":");
  PrintMethodCacheStateMethodBinding(instance_first_state);
  std::printf(",\"instance_second_state\":");
  PrintMethodCacheStateMethodBinding(instance_second_state);
  std::printf(",\"class_state\":");
  PrintMethodCacheStateMethodBinding(class_state);
  std::printf(",\"known_class_state\":");
  PrintMethodCacheStateMethodBinding(known_class_state);
  std::printf(",\"category_state\":");
  PrintMethodCacheStateMethodBinding(category_state);
  std::printf(",\"instance_entry\":");
  PrintMethodCacheEntryBasic(instance_entry);
  std::printf(",\"class_entry\":");
  PrintMethodCacheEntryBasic(class_entry);
  std::printf(",\"category_entry\":");
  PrintMethodCacheEntryBasic(category_entry);
  std::printf("}");
  return 0;
}
