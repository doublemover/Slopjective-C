#include "runtime/state/runtime_bootstrap_replay.h"

#include "runtime/metadata/runtime_ownership_contracts.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/state/runtime_live_state_reset.h"
#include "runtime/state/runtime_state_records.h"

#include <string>

namespace objc3c::runtime {

std::uint64_t ZeroRetainedBootstrapImageLocalInitStatesUnlocked(
    RuntimeState &state) {
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
  state.retired_route_path_allowed = RuntimeRetiredRoutePathsAreAllowed();
  state.last_replayed_image_count = 0;
  state.last_replayed_module_name.clear();
  state.last_replayed_translation_unit_identity_key.clear();

  if (register_image == nullptr || state.registered_image_count != 0 ||
      !state.registration_order_by_identity_key.empty() ||
      state.next_expected_registration_order_ordinal != 1 ||
      state.staged_registration_table != nullptr) {
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
      return state.last_replay_status;
    }

    const RegisteredImageMetadata &record = found->second;
    const int status = register_image(
        state, record.registration_table->image_descriptor,
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

}  // namespace objc3c::runtime
