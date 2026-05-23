#include "runtime/state/runtime_bootstrap_replay.h"

#include "runtime/metadata/runtime_ownership_contracts.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/state/runtime_live_state_reset.h"
#include "runtime/state/runtime_state_records.h"

#include <string>

namespace objc3c::runtime {

namespace {

bool RuntimeLiveRegistrationStateIsEmptyForReplayUnlocked(
    const RuntimeState &state) {
  return state.registered_image_count == 0 &&
         state.registered_descriptor_total == 0 &&
         state.registration_order_by_identity_key.empty() &&
         state.registered_image_metadata_by_identity_key.empty() &&
         state.next_expected_registration_order_ordinal == 1 &&
         state.staged_registration_table == nullptr &&
         state.selector_index_by_name.empty() && state.selector_slots.empty() &&
         state.keypath_slots.empty() &&
         state.metadata_backed_selector_count == 0 &&
         state.dynamic_selector_count == 0 &&
         state.metadata_provider_edge_count == 0 &&
         state.image_backed_keypath_count == 0 &&
         state.ambiguous_keypath_handle_count == 0 &&
         state.walked_image_count == 0 &&
         state.realized_class_name_by_base_identity.empty() &&
         state.ambiguous_realized_base_identities.empty() &&
         state.realized_class_node_indices_by_name.empty() &&
         state.realized_class_nodes.empty() &&
         state.realized_root_class_count == 0 &&
         state.realized_metaclass_edge_count == 0 &&
         state.receiver_class_binding_count == 0 &&
         state.realized_attached_category_count == 0 &&
         state.realized_protocol_conformance_edge_count == 0 &&
         state.method_cache.empty() && state.method_cache_hit_count == 0 &&
         state.method_cache_miss_count == 0 &&
         state.slow_path_lookup_count == 0 &&
         state.stale_method_cache_entry_count == 0 &&
         state.next_method_cache_entry_generation == 1 &&
         state.live_dispatch_count == 0 &&
         state.strict_dispatch_error_count == 0 &&
         state.fast_path_seed_count == 0 && state.fast_path_hit_count == 0 &&
         state.property_lookup_cache.empty() &&
         state.property_lookup_cache_hit_count == 0 &&
         state.property_lookup_cache_miss_count == 0;
}

} // namespace

std::uint64_t
ZeroRetainedBootstrapImageLocalInitStatesUnlocked(RuntimeState &state) {
  std::uint64_t cleared_count = 0;
  for (const std::string &identity_key :
       state.retained_bootstrap_identity_order) {
    const auto found =
        state.retained_bootstrap_metadata_by_identity_key.find(identity_key);
    if (found == state.retained_bootstrap_metadata_by_identity_key.end()) {
      continue;
    }
    const RegisteredImageMetadata &record = found->second;
    if (record.registration_table == nullptr ||
        record.registration_table->image_local_init_state == nullptr) {
      continue;
    }
    *record.registration_table->image_local_init_state = 0;
    ++cleared_count;
  }
  return cleared_count;
}

int ReplayRegisteredImagesForTestingUnlocked(
    RuntimeState &state,
    RuntimeImageRegistrationReplayCallback register_image) {
  state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.bootstrap_replay_owner = kObjc3RuntimeBootstrapReplayOwner;
  state.runtime_owner_split_explicit = RuntimeOwnerSplitContractIsReady();
  state.last_replayed_image_count = 0;
  state.last_replayed_module_name.clear();
  state.last_replayed_translation_unit_identity_key.clear();

  if (register_image == nullptr ||
      !RuntimeLiveRegistrationStateIsEmptyForReplayUnlocked(state)) {
    state.last_replay_status =
        OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
    return state.last_replay_status;
  }

  for (const std::string &identity_key :
       state.retained_bootstrap_identity_order) {
    const auto found =
        state.retained_bootstrap_metadata_by_identity_key.find(identity_key);
    if (found == state.retained_bootstrap_metadata_by_identity_key.end() ||
        found->second.registration_table == nullptr ||
        found->second.registration_table->image_descriptor == nullptr) {
      ClearLiveRegistrationStateUnlocked(state);
      state.last_reset_cleared_image_local_init_state_count =
          ZeroRetainedBootstrapImageLocalInitStatesUnlocked(state);
      state.last_replay_status =
          OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
      state.last_replayed_image_count = 0;
      state.last_replayed_module_name.clear();
      state.last_replayed_translation_unit_identity_key.clear();
      return state.last_replay_status;
    }

    const RegisteredImageMetadata &record = found->second;
    const int status =
        register_image(state, record.registration_table->image_descriptor,
                       record.registration_table, false, true);
    if (status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK) {
      ClearLiveRegistrationStateUnlocked(state);
      state.last_reset_cleared_image_local_init_state_count =
          ZeroRetainedBootstrapImageLocalInitStatesUnlocked(state);
      state.last_replay_status = status;
      state.last_replayed_image_count = 0;
      state.last_replayed_module_name.clear();
      state.last_replayed_translation_unit_identity_key.clear();
      return status;
    }

    ++state.last_replayed_image_count;
    state.last_replayed_module_name = record.module_name;
    state.last_replayed_translation_unit_identity_key =
        record.translation_unit_identity_key;
  }

  ++state.replay_generation;
  state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

} // namespace objc3c::runtime
