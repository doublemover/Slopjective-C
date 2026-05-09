#include "runtime/selectors/keypath_table.h"

#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/strings/borrowed_string.h"

#include <mutex>
#include <utility>

namespace objc3c::runtime {

namespace {

std::uint64_t CountKeyPathComponents(const char *component_path) {
  if (component_path == nullptr || component_path[0] == '\0') {
    return 0;
  }
  std::uint64_t count = 1;
  for (const unsigned char *cursor =
           reinterpret_cast<const unsigned char *>(component_path);
       *cursor != 0U; ++cursor) {
    if (*cursor == static_cast<unsigned char>('.')) {
      ++count;
    }
  }
  return count;
}

bool KeyPathDescriptorsCompatible(const KeyPathSlot &slot,
                                  const EmittedKeyPathDescriptor &descriptor) {
  const char *generic_metadata_replay_key =
      descriptor.generic_metadata_replay_key == nullptr
          ? ""
          : descriptor.generic_metadata_replay_key;
  return slot.root_name_storage == descriptor.root_name &&
         slot.component_path_storage == descriptor.component_path &&
         slot.profile_storage == descriptor.profile &&
         slot.generic_metadata_replay_key_storage ==
             generic_metadata_replay_key &&
         slot.root_is_self == descriptor.root_is_self;
}

}  // namespace

bool RuntimeKeyPathHandleIsValid(std::uint64_t stable_id) {
  return stable_id != 0;
}

bool RuntimeKeyPathDescriptorIsMaterializable(const char *root_name,
                                              const char *component_path) {
  return root_name != nullptr && root_name[0] != '\0' &&
         component_path != nullptr && component_path[0] != '\0';
}

bool MaterializeKeyPathDescriptorUnlocked(
    RuntimeState &state,
    const EmittedKeyPathDescriptor &descriptor,
    std::uint64_t registration_order_ordinal) {
  if (!RuntimeKeyPathHandleIsValid(descriptor.stable_id) ||
      !RuntimeKeyPathDescriptorIsMaterializable(descriptor.root_name,
                                                descriptor.component_path) ||
      descriptor.profile == nullptr || descriptor.profile[0] == '\0') {
    return false;
  }

  const auto found = state.keypath_slots.find(descriptor.stable_id);
  if (found == state.keypath_slots.end()) {
    KeyPathSlot slot;
    slot.stable_id = descriptor.stable_id;
    slot.root_name_storage = descriptor.root_name;
    slot.component_path_storage = descriptor.component_path;
    slot.profile_storage = descriptor.profile;
    slot.generic_metadata_replay_key_storage =
        descriptor.generic_metadata_replay_key == nullptr
            ? ""
            : descriptor.generic_metadata_replay_key;
    slot.root_is_self = descriptor.root_is_self;
    slot.component_count = CountKeyPathComponents(descriptor.component_path);
    slot.metadata_provider_count = 1;
    slot.first_registration_order_ordinal = registration_order_ordinal;
    slot.last_registration_order_ordinal = registration_order_ordinal;
    if (slot.component_count == 0) {
      return false;
    }
    state.keypath_slots.emplace(descriptor.stable_id, std::move(slot));
    ++state.image_backed_keypath_count;
    state.last_materialized_keypath_handle = descriptor.stable_id;
    state.last_materialized_keypath_registration_order_ordinal =
        registration_order_ordinal;
    state.last_materialized_keypath_profile = descriptor.profile;
    return true;
  }

  KeyPathSlot &slot = found->second;
  if (!KeyPathDescriptorsCompatible(slot, descriptor)) {
    if (!slot.ambiguous) {
      slot.ambiguous = true;
      ++state.ambiguous_keypath_handle_count;
    }
  }
  ++slot.metadata_provider_count;
  slot.last_registration_order_ordinal = registration_order_ordinal;
  state.last_materialized_keypath_handle = descriptor.stable_id;
  state.last_materialized_keypath_registration_order_ordinal =
      registration_order_ordinal;
  state.last_materialized_keypath_profile = slot.profile_storage;
  return true;
}

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_copy_keypath_registry_state_for_testing(
    objc3_runtime_keypath_registry_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->keypath_table_entry_count =
      static_cast<std::uint64_t>(state.keypath_slots.size());
  snapshot->image_backed_keypath_count = state.image_backed_keypath_count;
  snapshot->ambiguous_keypath_handle_count =
      state.ambiguous_keypath_handle_count;
  snapshot->last_materialized_handle = state.last_materialized_keypath_handle;
  snapshot->last_materialized_registration_order_ordinal =
      state.last_materialized_keypath_registration_order_ordinal;
  snapshot->last_queried_handle = state.last_queried_keypath_handle;
  snapshot->last_query_found = state.last_keypath_query_found ? 1 : 0;
  snapshot->last_query_ambiguous = state.last_keypath_query_ambiguous ? 1 : 0;
  snapshot->last_materialized_profile =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_materialized_keypath_profile);
  snapshot->last_resolved_profile =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_resolved_keypath_profile);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_keypath_entry_for_testing(
    std::uint64_t stable_id,
    objc3_runtime_keypath_entry_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->found = 0;
  snapshot->ambiguous = 0;
  snapshot->root_is_self = 0;
  snapshot->stable_id = stable_id;
  snapshot->component_count = 0;
  snapshot->metadata_provider_count = 0;
  snapshot->first_registration_order_ordinal = 0;
  snapshot->last_registration_order_ordinal = 0;
  snapshot->root_name = nullptr;
  snapshot->component_path = nullptr;
  snapshot->profile = nullptr;
  snapshot->generic_metadata_replay_key = nullptr;

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.last_queried_keypath_handle = stable_id;
  state.last_keypath_query_found = false;
  state.last_keypath_query_ambiguous = false;
  state.last_resolved_keypath_profile.clear();
  const auto found = state.keypath_slots.find(stable_id);
  if (found == state.keypath_slots.end()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  const objc3c::runtime::KeyPathSlot &slot = found->second;
  state.last_keypath_query_found = true;
  state.last_keypath_query_ambiguous = slot.ambiguous;
  state.last_resolved_keypath_profile = slot.profile_storage;
  snapshot->found = 1;
  snapshot->ambiguous = slot.ambiguous ? 1 : 0;
  snapshot->root_is_self = slot.root_is_self ? 1 : 0;
  snapshot->component_count = slot.component_count;
  snapshot->metadata_provider_count = slot.metadata_provider_count;
  snapshot->first_registration_order_ordinal =
      slot.first_registration_order_ordinal;
  snapshot->last_registration_order_ordinal =
      slot.last_registration_order_ordinal;
  snapshot->root_name =
      objc3c::runtime::BorrowRuntimeCString(slot.root_name_storage);
  snapshot->component_path =
      objc3c::runtime::BorrowRuntimeCString(slot.component_path_storage);
  snapshot->profile =
      objc3c::runtime::BorrowRuntimeCString(slot.profile_storage);
  snapshot->generic_metadata_replay_key =
      slot.generic_metadata_replay_key_storage.empty()
          ? nullptr
          : objc3c::runtime::BorrowRuntimeCString(
                slot.generic_metadata_replay_key_storage);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_keypath_component_count_for_testing(
    int keypath_handle) {
  if (keypath_handle <= 0 ||
      !objc3c::runtime::RuntimeKeyPathHandleIsValid(
          static_cast<std::uint64_t>(keypath_handle))) {
    return 0;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto found =
      state.keypath_slots.find(static_cast<std::uint64_t>(keypath_handle));
  state.last_queried_keypath_handle =
      static_cast<std::uint64_t>(keypath_handle);
  state.last_keypath_query_found = found != state.keypath_slots.end();
  state.last_keypath_query_ambiguous =
      found != state.keypath_slots.end() && found->second.ambiguous;
  state.last_resolved_keypath_profile =
      found != state.keypath_slots.end() ? found->second.profile_storage : "";
  if (found == state.keypath_slots.end() || found->second.ambiguous) {
    return 0;
  }
  return static_cast<int>(found->second.component_count);
}

extern "C" int objc3_runtime_keypath_root_is_self_for_testing(
    int keypath_handle) {
  if (keypath_handle <= 0 ||
      !objc3c::runtime::RuntimeKeyPathHandleIsValid(
          static_cast<std::uint64_t>(keypath_handle))) {
    return 0;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto found =
      state.keypath_slots.find(static_cast<std::uint64_t>(keypath_handle));
  state.last_queried_keypath_handle =
      static_cast<std::uint64_t>(keypath_handle);
  state.last_keypath_query_found = found != state.keypath_slots.end();
  state.last_keypath_query_ambiguous =
      found != state.keypath_slots.end() && found->second.ambiguous;
  state.last_resolved_keypath_profile =
      found != state.keypath_slots.end() ? found->second.profile_storage : "";
  if (found == state.keypath_slots.end() || found->second.ambiguous) {
    return 0;
  }
  return found->second.root_is_self ? 1 : 0;
}
