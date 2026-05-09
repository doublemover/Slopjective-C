#include "runtime/state/runtime_reset.h"

#include "runtime/blocks/block_runtime_state.h"
#include "runtime/concurrency/runtime_debug.h"
#include "runtime/errors/error_bridge.h"
#include "runtime/images/registration_snapshots.h"
#include "runtime/memory/arc_debug_state.h"
#include "runtime/memory/autorelease_pool.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/state/runtime_state.h"
#include "runtime/state/runtime_state_clear.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <mutex>
#include <string>

namespace objc3c::runtime {

void ResetRuntimeThreadLocalDebugStateForTesting() {
  ResetRuntimeAutoreleasepoolStateForTesting();
  ResetRuntimeArcDebugStateForTesting();
  ResetRuntimeBlockDebugStateForTesting();
  ResetRuntimeErrorBridgeStateForTesting();
  ResetRuntimeConcurrencyDebugStateForTesting();
}

bool RuntimeResetPreservesBootstrapCatalog() {
  return true;
}

bool RuntimeResetClearsLiveExecutionState() {
  return true;
}

std::uint64_t ZeroRetainedBootstrapImageLocalInitStatesUnlocked(
    RuntimeState &state) {
  std::uint64_t cleared_count = 0;
  for (const std::string &identity_key : state.retained_bootstrap_identity_order) {
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

void ClearLiveRegistrationStateUnlocked(RuntimeState &state) {
  state.registered_image_count = 0;
  state.registered_descriptor_total = 0;
  state.next_expected_registration_order_ordinal = 1;
  state.last_successful_registration_order_ordinal = 0;
  state.last_registration_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_registered_module_name.clear();
  state.last_registered_translation_unit_identity_key.clear();
  state.last_rejected_module_name.clear();
  state.last_rejected_translation_unit_identity_key.clear();
  state.last_rejected_registration_order_ordinal = 0;
  state.registration_order_by_identity_key.clear();
  state.registered_image_metadata_by_identity_key.clear();
  state.selector_index_by_name.clear();
  state.selector_slots.clear();
  state.metadata_backed_selector_count = 0;
  state.dynamic_selector_count = 0;
  state.metadata_provider_edge_count = 0;
  state.last_materialized_selector.clear();
  state.last_materialized_stable_id = 0;
  state.last_materialized_registration_order_ordinal = 0;
  state.last_materialized_selector_pool_index = 0;
  state.last_materialized_from_metadata = false;
  state.keypath_slots.clear();
  state.image_backed_keypath_count = 0;
  state.ambiguous_keypath_handle_count = 0;
  state.last_materialized_keypath_handle = 0;
  state.last_materialized_keypath_registration_order_ordinal = 0;
  state.last_materialized_keypath_profile.clear();
  state.last_queried_keypath_handle = 0;
  state.last_keypath_query_found = false;
  state.last_keypath_query_ambiguous = false;
  state.last_resolved_keypath_profile.clear();
  ClearMethodCacheStateUnlocked(state);
  ClearRealizedClassGraphUnlocked(state);
  ClearRuntimeInstanceStateUnlocked(state);
  state.staged_registration_table = nullptr;
  state.walked_image_count = 0;
  ClearImageWalkSnapshotUnlocked(state);
}

int ReplayRegisteredImagesForTestingUnlocked(
    RuntimeState &state,
    RuntimeImageRegistrationReplayCallback register_image) {
  state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
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

  for (const std::string &identity_key : state.retained_bootstrap_identity_order) {
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

extern "C" void objc3_runtime_reset_for_testing(void) {
  objc3c::runtime::RuntimeState &state =
      objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  (void)objc3c::runtime::RuntimeStateLockDiscipline();
  (void)objc3c::runtime::RuntimeStateOwnershipModel();
  if (objc3c::runtime::RuntimeResetClearsLiveExecutionState()) {
    objc3c::runtime::ClearLiveRegistrationStateUnlocked(state);
  }
  state.last_reset_cleared_image_local_init_state_count =
      objc3c::runtime::RuntimeResetPreservesBootstrapCatalog()
          ? objc3c::runtime::ZeroRetainedBootstrapImageLocalInitStatesUnlocked(
                state)
          : 0;
  ++state.reset_generation;
  state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_replayed_image_count = 0;
  state.last_replayed_module_name.clear();
  state.last_replayed_translation_unit_identity_key.clear();
  objc3c::runtime::ResetRuntimeThreadLocalDebugStateForTesting();
}
