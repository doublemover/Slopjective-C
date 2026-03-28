#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdint>
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

void StabilizeRegistrationState(
    objc3_runtime_registration_state_snapshot &snapshot,
    std::string &registered_module_storage,
    std::string &registered_identity_storage,
    std::string &rejected_module_storage,
    std::string &rejected_identity_storage) {
  StabilizeNullableCString(snapshot.last_registered_module_name,
                           registered_module_storage,
                           snapshot.last_registered_module_name);
  StabilizeNullableCString(snapshot.last_registered_translation_unit_identity_key,
                           registered_identity_storage,
                           snapshot.last_registered_translation_unit_identity_key);
  StabilizeNullableCString(snapshot.last_rejected_module_name,
                           rejected_module_storage,
                           snapshot.last_rejected_module_name);
  StabilizeNullableCString(snapshot.last_rejected_translation_unit_identity_key,
                           rejected_identity_storage,
                           snapshot.last_rejected_translation_unit_identity_key);
}

void StabilizeImageWalkState(objc3_runtime_image_walk_state_snapshot &snapshot,
                             std::string &module_storage,
                             std::string &identity_storage) {
  StabilizeNullableCString(snapshot.last_walked_module_name, module_storage,
                           snapshot.last_walked_module_name);
  StabilizeNullableCString(snapshot.last_walked_translation_unit_identity_key,
                           identity_storage,
                           snapshot.last_walked_translation_unit_identity_key);
}

void StabilizeResetReplayState(
    objc3_runtime_reset_replay_state_snapshot &snapshot,
    std::string &module_storage,
    std::string &identity_storage) {
  StabilizeNullableCString(snapshot.last_replayed_module_name, module_storage,
                           snapshot.last_replayed_module_name);
  StabilizeNullableCString(snapshot.last_replayed_translation_unit_identity_key,
                           identity_storage,
                           snapshot.last_replayed_translation_unit_identity_key);
}

void StabilizeSelectorTableState(
    objc3_runtime_selector_lookup_table_state_snapshot &snapshot,
    std::string &selector_storage) {
  StabilizeNullableCString(snapshot.last_materialized_selector, selector_storage,
                           snapshot.last_materialized_selector);
}

void StabilizeSelectorEntry(
    objc3_runtime_selector_lookup_entry_snapshot &snapshot,
    std::string &selector_storage) {
  StabilizeNullableCString(snapshot.canonical_selector, selector_storage,
                           snapshot.canonical_selector);
}

void PrintSelectorTableState(
    const objc3_runtime_selector_lookup_table_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"selector_table_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.selector_table_entry_count));
  std::printf("\"metadata_backed_selector_count\":%llu,",
              static_cast<unsigned long long>(snapshot.metadata_backed_selector_count));
  std::printf("\"dynamic_selector_count\":%llu,",
              static_cast<unsigned long long>(snapshot.dynamic_selector_count));
  std::printf("\"metadata_provider_edge_count\":%llu,",
              static_cast<unsigned long long>(snapshot.metadata_provider_edge_count));
  std::printf("\"last_materialized_selector\":");
  PrintJsonStringOrNull(snapshot.last_materialized_selector);
  std::printf(",\"last_materialized_stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.last_materialized_stable_id));
  std::printf(
      "\"last_materialized_registration_order_ordinal\":%llu,",
      static_cast<unsigned long long>(
          snapshot.last_materialized_registration_order_ordinal));
  std::printf("\"last_materialized_selector_pool_index\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_materialized_selector_pool_index));
  std::printf("\"last_materialized_from_metadata\":%d",
              snapshot.last_materialized_from_metadata);
  std::printf("}");
}

void PrintSelectorEntry(
    const objc3_runtime_selector_lookup_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"metadata_backed\":%d,", snapshot.metadata_backed);
  std::printf("\"stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.stable_id));
  std::printf("\"metadata_provider_count\":%llu,",
              static_cast<unsigned long long>(snapshot.metadata_provider_count));
  std::printf("\"first_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.first_registration_order_ordinal));
  std::printf("\"last_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_registration_order_ordinal));
  std::printf("\"first_selector_pool_index\":%llu,",
              static_cast<unsigned long long>(snapshot.first_selector_pool_index));
  std::printf("\"last_selector_pool_index\":%llu,",
              static_cast<unsigned long long>(snapshot.last_selector_pool_index));
  std::printf("\"canonical_selector\":");
  PrintJsonStringOrNull(snapshot.canonical_selector);
  std::printf("}");
}

void PrintRegistrationState(
    const objc3_runtime_registration_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"registered_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_image_count));
  std::printf("\"registered_descriptor_total\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_descriptor_total));
  std::printf(
      "\"next_expected_registration_order_ordinal\":%llu,",
      static_cast<unsigned long long>(
          snapshot.next_expected_registration_order_ordinal));
  std::printf(
      "\"last_successful_registration_order_ordinal\":%llu,",
      static_cast<unsigned long long>(
          snapshot.last_successful_registration_order_ordinal));
  std::printf("\"last_registration_status\":%d,",
              snapshot.last_registration_status);
  std::printf("\"last_registered_module_name\":");
  PrintJsonStringOrNull(snapshot.last_registered_module_name);
  std::printf(",\"last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(snapshot.last_registered_translation_unit_identity_key);
  std::printf(",\"last_rejected_registration_order_ordinal\":%llu",
              static_cast<unsigned long long>(
                  snapshot.last_rejected_registration_order_ordinal));
  std::printf("}");
}

void PrintImageWalkState(
    const objc3_runtime_image_walk_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"walked_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.walked_image_count));
  std::printf(
      "\"last_walked_selector_pool_count\":%llu,",
      static_cast<unsigned long long>(snapshot.last_walked_selector_pool_count));
  std::printf(
      "\"last_registration_used_staged_table\":%d,",
      snapshot.last_registration_used_staged_table);
  std::printf("\"last_walked_translation_unit_identity_key\":");
  PrintJsonStringOrNull(snapshot.last_walked_translation_unit_identity_key);
  std::printf("}");
}

void PrintResetReplayState(
    const objc3_runtime_reset_replay_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"retained_bootstrap_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.retained_bootstrap_image_count));
  std::printf(
      "\"last_reset_cleared_image_local_init_state_count\":%llu,",
      static_cast<unsigned long long>(
          snapshot.last_reset_cleared_image_local_init_state_count));
  std::printf("\"last_replayed_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.last_replayed_image_count));
  std::printf("\"reset_generation\":%llu,",
              static_cast<unsigned long long>(snapshot.reset_generation));
  std::printf("\"replay_generation\":%llu,",
              static_cast<unsigned long long>(snapshot.replay_generation));
  std::printf("\"last_replay_status\":%d,", snapshot.last_replay_status);
  std::printf("\"last_replayed_translation_unit_identity_key\":");
  PrintJsonStringOrNull(snapshot.last_replayed_translation_unit_identity_key);
  std::printf("}");
}

constexpr char kManualModuleName[] = "dispatch_runtime-d002-manual-image";
constexpr char kManualIdentityKey[] = "dispatch_runtime-d002::manual-image";
constexpr char kManualSelectorTokenValue[] = "tokenValue";
constexpr char kManualSelectorDebugName[] = "debugName";
constexpr char kManualStringDebugName[] = "debugName";
constexpr char kDynamicSelector[] = "manualOnly:";

template <size_t EntryCount>
struct PointerAggregateStorage {
  uint64_t count;
  const void *entries[EntryCount];
};

const objc3_runtime_image_descriptor kManualImageDescriptor{
    kManualModuleName,
    kManualIdentityKey,
    2,
    0,
    0,
    0,
    0,
    0,
};

const PointerAggregateStorage<1> kManualEmptyClassRootStorage = {0, {nullptr}};
const PointerAggregateStorage<1> kManualEmptyProtocolRootStorage = {0, {nullptr}};
const PointerAggregateStorage<1> kManualEmptyCategoryRootStorage = {0, {nullptr}};
const PointerAggregateStorage<1> kManualEmptyPropertyRootStorage = {0, {nullptr}};
const PointerAggregateStorage<1> kManualEmptyIvarRootStorage = {0, {nullptr}};
const PointerAggregateStorage<2> kManualSelectorPoolRootStorage = {
    2,
    {kManualSelectorTokenValue, kManualSelectorDebugName},
};
const PointerAggregateStorage<1> kManualStringPoolRootStorage = {
    1,
    {kManualStringDebugName},
};
const PointerAggregateStorage<7> kManualDiscoveryRootStorage = {
    7,
    {
        &kManualEmptyClassRootStorage,
        &kManualEmptyProtocolRootStorage,
        &kManualEmptyCategoryRootStorage,
        &kManualEmptyPropertyRootStorage,
        &kManualEmptyIvarRootStorage,
        &kManualSelectorPoolRootStorage,
        &kManualStringPoolRootStorage,
    },
};
const objc3_runtime_pointer_aggregate *kManualEmptyClassRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualEmptyClassRootStorage);
const objc3_runtime_pointer_aggregate *kManualEmptyProtocolRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualEmptyProtocolRootStorage);
const objc3_runtime_pointer_aggregate *kManualEmptyCategoryRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualEmptyCategoryRootStorage);
const objc3_runtime_pointer_aggregate *kManualEmptyPropertyRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualEmptyPropertyRootStorage);
const objc3_runtime_pointer_aggregate *kManualEmptyIvarRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualEmptyIvarRootStorage);
const objc3_runtime_pointer_aggregate *kManualSelectorPoolRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualSelectorPoolRootStorage);
const objc3_runtime_pointer_aggregate *kManualStringPoolRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualStringPoolRootStorage);
const objc3_runtime_pointer_aggregate *kManualDiscoveryRoot =
    reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
        &kManualDiscoveryRootStorage);
const void *kManualDiscoveryRootAnchorStorage = kManualDiscoveryRoot;
unsigned char kManualImageLocalInitState = 0;
const objc3_runtime_registration_table kManualRegistrationTable = {
    1,
    11,
    &kManualImageDescriptor,
    kManualDiscoveryRoot,
    &kManualDiscoveryRootAnchorStorage,
    kManualEmptyClassRoot,
    kManualEmptyProtocolRoot,
    kManualEmptyCategoryRoot,
    kManualEmptyPropertyRoot,
    kManualEmptyIvarRoot,
    kManualSelectorPoolRoot,
    kManualStringPoolRoot,
    &kManualImageLocalInitState,
};

int CopySelectorEntry(const char *selector,
                      objc3_runtime_selector_lookup_entry_snapshot *snapshot) {
  return objc3_runtime_copy_selector_lookup_entry_for_testing(selector, snapshot);
}

}  // namespace

int main() {
  objc3_runtime_registration_state_snapshot startup_registration{};
  objc3_runtime_image_walk_state_snapshot startup_image_walk{};
  objc3_runtime_selector_lookup_table_state_snapshot startup_table{};
  objc3_runtime_selector_lookup_entry_snapshot startup_token{};
  objc3_runtime_selector_lookup_entry_snapshot startup_current{};
  objc3_runtime_selector_lookup_entry_snapshot startup_set_current{};
  objc3_runtime_selector_lookup_entry_snapshot startup_shared{};
  objc3_runtime_selector_lookup_entry_snapshot startup_manual_only_before{};
  std::string startup_registration_module_storage;
  std::string startup_registration_identity_storage;
  std::string startup_registration_rejected_module_storage;
  std::string startup_registration_rejected_identity_storage;
  std::string startup_image_walk_module_storage;
  std::string startup_image_walk_identity_storage;
  std::string startup_table_selector_storage;
  std::string startup_token_selector_storage;
  std::string startup_current_selector_storage;
  std::string startup_set_current_selector_storage;
  std::string startup_shared_selector_storage;
  std::string startup_manual_only_before_selector_storage;

  const int startup_registration_status =
      objc3_runtime_copy_registration_state_for_testing(&startup_registration);
  const int startup_image_walk_status =
      objc3_runtime_copy_image_walk_state_for_testing(&startup_image_walk);
  const int startup_table_status =
      objc3_runtime_copy_selector_lookup_table_state_for_testing(&startup_table);
  const int startup_token_status = CopySelectorEntry("tokenValue", &startup_token);
  const int startup_current_status =
      CopySelectorEntry("currentValue", &startup_current);
  const int startup_set_current_status =
      CopySelectorEntry("setCurrentValue:", &startup_set_current);
  const int startup_shared_status = CopySelectorEntry("shared", &startup_shared);
  const int startup_manual_only_before_status =
      CopySelectorEntry(kDynamicSelector, &startup_manual_only_before);
  StabilizeRegistrationState(startup_registration,
                             startup_registration_module_storage,
                             startup_registration_identity_storage,
                             startup_registration_rejected_module_storage,
                             startup_registration_rejected_identity_storage);
  StabilizeImageWalkState(startup_image_walk, startup_image_walk_module_storage,
                          startup_image_walk_identity_storage);
  StabilizeSelectorTableState(startup_table, startup_table_selector_storage);
  StabilizeSelectorEntry(startup_token, startup_token_selector_storage);
  StabilizeSelectorEntry(startup_current, startup_current_selector_storage);
  StabilizeSelectorEntry(startup_set_current,
                         startup_set_current_selector_storage);
  StabilizeSelectorEntry(startup_shared, startup_shared_selector_storage);
  StabilizeSelectorEntry(startup_manual_only_before,
                         startup_manual_only_before_selector_storage);

  objc3_runtime_stage_registration_table_for_bootstrap(&kManualRegistrationTable);
  const int manual_register_status =
      objc3_runtime_register_image(&kManualImageDescriptor);

  objc3_runtime_registration_state_snapshot after_manual_registration{};
  objc3_runtime_image_walk_state_snapshot after_manual_image_walk{};
  objc3_runtime_selector_lookup_table_state_snapshot after_manual_table{};
  objc3_runtime_selector_lookup_entry_snapshot after_manual_token{};
  objc3_runtime_selector_lookup_entry_snapshot after_manual_debug_name{};
  std::string after_manual_registration_module_storage;
  std::string after_manual_registration_identity_storage;
  std::string after_manual_registration_rejected_module_storage;
  std::string after_manual_registration_rejected_identity_storage;
  std::string after_manual_image_walk_module_storage;
  std::string after_manual_image_walk_identity_storage;
  std::string after_manual_table_selector_storage;
  std::string after_manual_token_selector_storage;
  std::string after_manual_debug_name_selector_storage;
  const int after_manual_registration_status =
      objc3_runtime_copy_registration_state_for_testing(&after_manual_registration);
  const int after_manual_image_walk_status =
      objc3_runtime_copy_image_walk_state_for_testing(&after_manual_image_walk);
  const int after_manual_table_status =
      objc3_runtime_copy_selector_lookup_table_state_for_testing(&after_manual_table);
  const int after_manual_token_status =
      CopySelectorEntry("tokenValue", &after_manual_token);
  const int after_manual_debug_name_status =
      CopySelectorEntry("debugName", &after_manual_debug_name);
  StabilizeRegistrationState(after_manual_registration,
                             after_manual_registration_module_storage,
                             after_manual_registration_identity_storage,
                             after_manual_registration_rejected_module_storage,
                             after_manual_registration_rejected_identity_storage);
  StabilizeImageWalkState(after_manual_image_walk,
                          after_manual_image_walk_module_storage,
                          after_manual_image_walk_identity_storage);
  StabilizeSelectorTableState(after_manual_table,
                              after_manual_table_selector_storage);
  StabilizeSelectorEntry(after_manual_token,
                         after_manual_token_selector_storage);
  StabilizeSelectorEntry(after_manual_debug_name,
                         after_manual_debug_name_selector_storage);

  const objc3_runtime_selector_handle *dynamic_handle =
      objc3_runtime_lookup_selector(kDynamicSelector);
  const std::uint64_t dynamic_handle_stable_id =
      dynamic_handle != nullptr ? dynamic_handle->stable_id : 0;
  objc3_runtime_selector_lookup_table_state_snapshot after_dynamic_table{};
  objc3_runtime_selector_lookup_entry_snapshot after_dynamic_manual_only{};
  std::string after_dynamic_table_selector_storage;
  std::string after_dynamic_manual_only_selector_storage;
  const int after_dynamic_table_status =
      objc3_runtime_copy_selector_lookup_table_state_for_testing(&after_dynamic_table);
  const int after_dynamic_manual_only_status =
      CopySelectorEntry(kDynamicSelector, &after_dynamic_manual_only);
  StabilizeSelectorTableState(after_dynamic_table,
                              after_dynamic_table_selector_storage);
  StabilizeSelectorEntry(after_dynamic_manual_only,
                         after_dynamic_manual_only_selector_storage);

  objc3_runtime_reset_for_testing();

  objc3_runtime_registration_state_snapshot after_reset_registration{};
  objc3_runtime_selector_lookup_table_state_snapshot after_reset_table{};
  std::string after_reset_registration_module_storage;
  std::string after_reset_registration_identity_storage;
  std::string after_reset_registration_rejected_module_storage;
  std::string after_reset_registration_rejected_identity_storage;
  std::string after_reset_table_selector_storage;
  const int after_reset_registration_status =
      objc3_runtime_copy_registration_state_for_testing(&after_reset_registration);
  const int after_reset_table_status =
      objc3_runtime_copy_selector_lookup_table_state_for_testing(&after_reset_table);
  StabilizeRegistrationState(after_reset_registration,
                             after_reset_registration_module_storage,
                             after_reset_registration_identity_storage,
                             after_reset_registration_rejected_module_storage,
                             after_reset_registration_rejected_identity_storage);
  StabilizeSelectorTableState(after_reset_table,
                              after_reset_table_selector_storage);

  const int replay_status = objc3_runtime_replay_registered_images_for_testing();

  objc3_runtime_registration_state_snapshot after_replay_registration{};
  objc3_runtime_image_walk_state_snapshot after_replay_image_walk{};
  objc3_runtime_reset_replay_state_snapshot after_replay_reset_replay{};
  objc3_runtime_selector_lookup_table_state_snapshot after_replay_table{};
  objc3_runtime_selector_lookup_entry_snapshot after_replay_token{};
  objc3_runtime_selector_lookup_entry_snapshot after_replay_debug_name{};
  objc3_runtime_selector_lookup_entry_snapshot after_replay_manual_only_before{};
  std::string after_replay_registration_module_storage;
  std::string after_replay_registration_identity_storage;
  std::string after_replay_registration_rejected_module_storage;
  std::string after_replay_registration_rejected_identity_storage;
  std::string after_replay_image_walk_module_storage;
  std::string after_replay_image_walk_identity_storage;
  std::string after_replay_reset_replay_module_storage;
  std::string after_replay_reset_replay_identity_storage;
  std::string after_replay_table_selector_storage;
  std::string after_replay_token_selector_storage;
  std::string after_replay_debug_name_selector_storage;
  std::string after_replay_manual_only_before_selector_storage;
  const int after_replay_registration_status =
      objc3_runtime_copy_registration_state_for_testing(&after_replay_registration);
  const int after_replay_image_walk_status =
      objc3_runtime_copy_image_walk_state_for_testing(&after_replay_image_walk);
  const int after_replay_reset_replay_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&after_replay_reset_replay);
  const int after_replay_table_status =
      objc3_runtime_copy_selector_lookup_table_state_for_testing(&after_replay_table);
  const int after_replay_token_status =
      CopySelectorEntry("tokenValue", &after_replay_token);
  const int after_replay_debug_name_status =
      CopySelectorEntry("debugName", &after_replay_debug_name);
  const int after_replay_manual_only_before_status =
      CopySelectorEntry(kDynamicSelector, &after_replay_manual_only_before);
  StabilizeRegistrationState(after_replay_registration,
                             after_replay_registration_module_storage,
                             after_replay_registration_identity_storage,
                             after_replay_registration_rejected_module_storage,
                             after_replay_registration_rejected_identity_storage);
  StabilizeImageWalkState(after_replay_image_walk,
                          after_replay_image_walk_module_storage,
                          after_replay_image_walk_identity_storage);
  StabilizeResetReplayState(after_replay_reset_replay,
                            after_replay_reset_replay_module_storage,
                            after_replay_reset_replay_identity_storage);
  StabilizeSelectorTableState(after_replay_table,
                              after_replay_table_selector_storage);
  StabilizeSelectorEntry(after_replay_token,
                         after_replay_token_selector_storage);
  StabilizeSelectorEntry(after_replay_debug_name,
                         after_replay_debug_name_selector_storage);
  StabilizeSelectorEntry(after_replay_manual_only_before,
                         after_replay_manual_only_before_selector_storage);

  const objc3_runtime_selector_handle *replayed_dynamic_handle =
      objc3_runtime_lookup_selector(kDynamicSelector);
  const std::uint64_t replayed_dynamic_handle_stable_id =
      replayed_dynamic_handle != nullptr ? replayed_dynamic_handle->stable_id : 0;
  objc3_runtime_selector_lookup_table_state_snapshot after_replay_dynamic_table{};
  objc3_runtime_selector_lookup_entry_snapshot after_replay_dynamic_manual_only{};
  std::string after_replay_dynamic_table_selector_storage;
  std::string after_replay_dynamic_manual_only_selector_storage;
  const int after_replay_dynamic_table_status =
      objc3_runtime_copy_selector_lookup_table_state_for_testing(
          &after_replay_dynamic_table);
  const int after_replay_dynamic_manual_only_status =
      CopySelectorEntry(kDynamicSelector, &after_replay_dynamic_manual_only);
  StabilizeSelectorTableState(after_replay_dynamic_table,
                              after_replay_dynamic_table_selector_storage);
  StabilizeSelectorEntry(after_replay_dynamic_manual_only,
                         after_replay_dynamic_manual_only_selector_storage);

  std::printf("{");
  std::printf("\"startup_registration_status\":%d,", startup_registration_status);
  std::printf("\"startup_image_walk_status\":%d,", startup_image_walk_status);
  std::printf("\"startup_table_status\":%d,", startup_table_status);
  std::printf("\"startup_token_status\":%d,", startup_token_status);
  std::printf("\"startup_current_status\":%d,", startup_current_status);
  std::printf("\"startup_set_current_status\":%d,", startup_set_current_status);
  std::printf("\"startup_shared_status\":%d,", startup_shared_status);
  std::printf("\"startup_manual_only_before_status\":%d,", startup_manual_only_before_status);
  std::printf("\"manual_register_status\":%d,", manual_register_status);
  std::printf("\"after_manual_registration_status\":%d,", after_manual_registration_status);
  std::printf("\"after_manual_image_walk_status\":%d,", after_manual_image_walk_status);
  std::printf("\"after_manual_table_status\":%d,", after_manual_table_status);
  std::printf("\"after_manual_token_status\":%d,", after_manual_token_status);
  std::printf("\"after_manual_debug_name_status\":%d,", after_manual_debug_name_status);
  std::printf("\"after_dynamic_table_status\":%d,", after_dynamic_table_status);
  std::printf("\"after_dynamic_manual_only_status\":%d,", after_dynamic_manual_only_status);
  std::printf("\"after_reset_registration_status\":%d,", after_reset_registration_status);
  std::printf("\"after_reset_table_status\":%d,", after_reset_table_status);
  std::printf("\"replay_status\":%d,", replay_status);
  std::printf("\"after_replay_registration_status\":%d,", after_replay_registration_status);
  std::printf("\"after_replay_image_walk_status\":%d,", after_replay_image_walk_status);
  std::printf("\"after_replay_reset_replay_status\":%d,", after_replay_reset_replay_status);
  std::printf("\"after_replay_table_status\":%d,", after_replay_table_status);
  std::printf("\"after_replay_token_status\":%d,", after_replay_token_status);
  std::printf("\"after_replay_debug_name_status\":%d,", after_replay_debug_name_status);
  std::printf("\"after_replay_manual_only_before_status\":%d,", after_replay_manual_only_before_status);
  std::printf("\"after_replay_dynamic_table_status\":%d,", after_replay_dynamic_table_status);
  std::printf("\"after_replay_dynamic_manual_only_status\":%d,", after_replay_dynamic_manual_only_status);
  std::printf("\"dynamic_handle_stable_id\":%llu,",
              static_cast<unsigned long long>(dynamic_handle_stable_id));
  std::printf("\"replayed_dynamic_handle_stable_id\":%llu,",
              static_cast<unsigned long long>(replayed_dynamic_handle_stable_id));

  std::printf("\"startup_registration\":");
  PrintRegistrationState(startup_registration);
  std::printf(",\"startup_image_walk\":");
  PrintImageWalkState(startup_image_walk);
  std::printf(",\"startup_table\":");
  PrintSelectorTableState(startup_table);
  std::printf(",\"startup_token\":");
  PrintSelectorEntry(startup_token);
  std::printf(",\"startup_current\":");
  PrintSelectorEntry(startup_current);
  std::printf(",\"startup_set_current\":");
  PrintSelectorEntry(startup_set_current);
  std::printf(",\"startup_shared\":");
  PrintSelectorEntry(startup_shared);
  std::printf(",\"startup_manual_only_before\":");
  PrintSelectorEntry(startup_manual_only_before);

  std::printf(",\"after_manual_registration\":");
  PrintRegistrationState(after_manual_registration);
  std::printf(",\"after_manual_image_walk\":");
  PrintImageWalkState(after_manual_image_walk);
  std::printf(",\"after_manual_table\":");
  PrintSelectorTableState(after_manual_table);
  std::printf(",\"after_manual_token\":");
  PrintSelectorEntry(after_manual_token);
  std::printf(",\"after_manual_debug_name\":");
  PrintSelectorEntry(after_manual_debug_name);

  std::printf(",\"after_dynamic_table\":");
  PrintSelectorTableState(after_dynamic_table);
  std::printf(",\"after_dynamic_manual_only\":");
  PrintSelectorEntry(after_dynamic_manual_only);

  std::printf(",\"after_reset_registration\":");
  PrintRegistrationState(after_reset_registration);
  std::printf(",\"after_reset_table\":");
  PrintSelectorTableState(after_reset_table);

  std::printf(",\"after_replay_registration\":");
  PrintRegistrationState(after_replay_registration);
  std::printf(",\"after_replay_image_walk\":");
  PrintImageWalkState(after_replay_image_walk);
  std::printf(",\"after_replay_reset_replay\":");
  PrintResetReplayState(after_replay_reset_replay);
  std::printf(",\"after_replay_table\":");
  PrintSelectorTableState(after_replay_table);
  std::printf(",\"after_replay_token\":");
  PrintSelectorEntry(after_replay_token);
  std::printf(",\"after_replay_debug_name\":");
  PrintSelectorEntry(after_replay_debug_name);
  std::printf(",\"after_replay_manual_only_before\":");
  PrintSelectorEntry(after_replay_manual_only_before);
  std::printf(",\"after_replay_dynamic_table\":");
  PrintSelectorTableState(after_replay_dynamic_table);
  std::printf(",\"after_replay_dynamic_manual_only\":");
  PrintSelectorEntry(after_replay_dynamic_manual_only);
  std::printf("}\n");
  return 0;
}
