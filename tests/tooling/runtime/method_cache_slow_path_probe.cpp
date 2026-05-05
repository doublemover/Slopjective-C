#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/dispatch_expectations.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_json.h"
#include "support/runtime_snapshot_stabilizers.h"

#include <cstdio>
#include <string>

extern "C" int objc3_method_Widget_instance_callCurrentValue(void);
extern "C" int objc3_method_Widget_class_callSharedThroughSelf(void);
extern "C" int callSharedThroughKnownClass(void);

namespace {

using objc3c::runtime::probe::ExpectedStrictDispatchErrorValue;

using objc3c::runtime::probe::PrintMethodCacheEntryBasic;
using objc3c::runtime::probe::PrintMethodCacheStateSlowPath;
using objc3c::runtime::probe::PrintRegistrationStateBasic;
using objc3c::runtime::probe::PrintSelectorTableStateBasic;

using objc3c::runtime::probe::StabilizeMethodCacheEntry;
using objc3c::runtime::probe::StabilizeMethodCacheState;
using objc3c::runtime::probe::StabilizeNullableCString;
using objc3c::runtime::probe::StabilizeRegistrationState;
using objc3c::runtime::probe::StabilizeSelectorTableState;

using objc3c::runtime::probe::PrintJsonStringOrNull;

} // namespace

int main() {
  objc3_runtime_registration_state_snapshot registration_state{};
  objc3_runtime_selector_lookup_table_state_snapshot selector_table_state{};
  objc3_runtime_method_cache_state_snapshot instance_first_state{};
  objc3_runtime_method_cache_state_snapshot instance_second_state{};
  objc3_runtime_method_cache_state_snapshot class_self_state{};
  objc3_runtime_method_cache_state_snapshot known_class_state{};
  objc3_runtime_method_cache_state_snapshot strict_error_first_state{};
  objc3_runtime_method_cache_state_snapshot strict_error_second_state{};
  objc3_runtime_method_cache_entry_snapshot instance_entry{};
  objc3_runtime_method_cache_entry_snapshot class_entry{};
  objc3_runtime_method_cache_entry_snapshot strict_error_entry{};

  (void)objc3_runtime_copy_registration_state_for_testing(&registration_state);
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &selector_table_state);
  std::string registration_module_storage;
  std::string registration_identity_storage;
  std::string selector_table_last_storage;
  std::string instance_first_selector_storage;
  std::string instance_first_class_storage;
  std::string instance_first_owner_storage;
  std::string instance_second_selector_storage;
  std::string instance_second_class_storage;
  std::string instance_second_owner_storage;
  std::string class_self_selector_storage;
  std::string class_self_class_storage;
  std::string class_self_owner_storage;
  std::string known_class_selector_storage;
  std::string known_class_class_storage;
  std::string known_class_owner_storage;
  std::string strict_error_first_selector_storage;
  std::string strict_error_first_class_storage;
  std::string strict_error_first_owner_storage;
  std::string strict_error_second_selector_storage;
  std::string strict_error_second_class_storage;
  std::string strict_error_second_owner_storage;
  std::string instance_entry_selector_storage;
  std::string instance_entry_class_storage;
  std::string instance_entry_owner_storage;
  std::string class_entry_selector_storage;
  std::string class_entry_class_storage;
  std::string class_entry_owner_storage;
  std::string strict_error_entry_selector_storage;
  std::string strict_error_entry_class_storage;
  std::string strict_error_entry_owner_storage;

  StabilizeRegistrationState(registration_state, registration_module_storage,
                             registration_identity_storage);
  StabilizeSelectorTableState(selector_table_state,
                              selector_table_last_storage);
  const int instance_first = objc3_method_Widget_instance_callCurrentValue();
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &instance_first_state);
  StabilizeMethodCacheState(
      instance_first_state, instance_first_selector_storage,
      instance_first_class_storage, instance_first_owner_storage);
  const int instance_second = objc3_method_Widget_instance_callCurrentValue();
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &instance_second_state);
  StabilizeMethodCacheState(
      instance_second_state, instance_second_selector_storage,
      instance_second_class_storage, instance_second_owner_storage);
  const int class_self = objc3_method_Widget_class_callSharedThroughSelf();
  (void)objc3_runtime_copy_method_cache_state_for_testing(&class_self_state);
  StabilizeMethodCacheState(class_self_state, class_self_selector_storage,
                            class_self_class_storage, class_self_owner_storage);
  const int known_class = callSharedThroughKnownClass();
  (void)objc3_runtime_copy_method_cache_state_for_testing(&known_class_state);
  StabilizeMethodCacheState(known_class_state, known_class_selector_storage,
                            known_class_class_storage,
                            known_class_owner_storage);
  const char *const strict_error_selector = "missingRuntimeSelector:";
  const int strict_error_first =
      objc3_runtime_dispatch_i32(1025, strict_error_selector, 4, 5, 6, 7);
  const int strict_error_expected =
      ExpectedStrictDispatchErrorValue(1025, strict_error_selector, 4, 5, 6, 7);
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &strict_error_first_state);
  StabilizeMethodCacheState(
      strict_error_first_state, strict_error_first_selector_storage,
      strict_error_first_class_storage, strict_error_first_owner_storage);
  const int strict_error_second =
      objc3_runtime_dispatch_i32(1025, strict_error_selector, 4, 5, 6, 7);
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &strict_error_second_state);
  StabilizeMethodCacheState(
      strict_error_second_state, strict_error_second_selector_storage,
      strict_error_second_class_storage, strict_error_second_owner_storage);

  (void)objc3_runtime_copy_method_cache_entry_for_testing(1025, "currentValue",
                                                          &instance_entry);
  StabilizeMethodCacheEntry(instance_entry, instance_entry_selector_storage,
                            instance_entry_class_storage,
                            instance_entry_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(1024, "shared",
                                                          &class_entry);
  StabilizeMethodCacheEntry(class_entry, class_entry_selector_storage,
                            class_entry_class_storage,
                            class_entry_owner_storage);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      1025, strict_error_selector, &strict_error_entry);
  StabilizeMethodCacheEntry(strict_error_entry, strict_error_entry_selector_storage,
                            strict_error_entry_class_storage,
                            strict_error_entry_owner_storage);

  std::printf("{");
  std::printf("\"registration_state\":");
  PrintRegistrationStateBasic(registration_state);
  std::printf(",\"selector_table_state\":");
  PrintSelectorTableStateBasic(selector_table_state);
  std::printf(",\"instance_first\":%d,", instance_first);
  std::printf("\"instance_second\":%d,", instance_second);
  std::printf("\"class_self\":%d,", class_self);
  std::printf("\"known_class\":%d,", known_class);
  std::printf("\"strict_error_first\":%d,", strict_error_first);
  std::printf("\"strict_error_second\":%d,", strict_error_second);
  std::printf("\"strict_error_expected\":%d,", strict_error_expected);
  std::printf("\"instance_first_state\":");
  PrintMethodCacheStateSlowPath(instance_first_state);
  std::printf(",\"instance_second_state\":");
  PrintMethodCacheStateSlowPath(instance_second_state);
  std::printf(",\"class_self_state\":");
  PrintMethodCacheStateSlowPath(class_self_state);
  std::printf(",\"known_class_state\":");
  PrintMethodCacheStateSlowPath(known_class_state);
  std::printf(",\"strict_error_first_state\":");
  PrintMethodCacheStateSlowPath(strict_error_first_state);
  std::printf(",\"strict_error_second_state\":");
  PrintMethodCacheStateSlowPath(strict_error_second_state);
  std::printf(",\"instance_entry\":");
  PrintMethodCacheEntryBasic(instance_entry);
  std::printf(",\"class_entry\":");
  PrintMethodCacheEntryBasic(class_entry);
  std::printf(",\"strict_error_entry\":");
  PrintMethodCacheEntryBasic(strict_error_entry);
  std::printf("}\n");
  return 0;
}
