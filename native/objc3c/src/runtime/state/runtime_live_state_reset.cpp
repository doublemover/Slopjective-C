#include "runtime/state/runtime_live_state_reset.h"

#include "runtime/images/registration_snapshots.h"
#include "runtime/metadata/runtime_ownership_contracts.h"
#include "runtime/state/runtime_state_clear.h"
#include "runtime/state/runtime_state_records.h"

namespace objc3c::runtime {

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
  state.owner_split_contract_id = kObjc3RuntimeOwnerSplitContractId;
  state.metadata_model_owner = kObjc3RuntimeMetadataModelOwner;
  state.registration_table_owner = kObjc3RuntimeRegistrationTableOwner;
  state.manifest_descriptor_artifact_owner =
      kObjc3RuntimeManifestDescriptorArtifactOwner;
  state.bootstrap_replay_owner = kObjc3RuntimeBootstrapReplayOwner;
  state.dispatch_frame_state_owner = kObjc3RuntimeDispatchFrameStateOwner;
  state.public_registration_api_owner =
      kObjc3RuntimePublicRegistrationApiOwner;
  state.public_dispatch_diagnostics_owner =
      kObjc3RuntimePublicDispatchDiagnosticsOwner;
  state.fail_closed_ownership_model = kObjc3RuntimeFailClosedOwnershipModel;
  state.runtime_owner_split_explicit = RuntimeOwnerSplitContractIsReady();
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
  state.malformed_class_metadata_rejection_count = 0;
  state.last_malformed_class_graph_reason.clear();
  ClearMethodCacheStateUnlocked(state);
  ClearRealizedClassGraphUnlocked(state);
  ClearRuntimeInstanceStateUnlocked(state);
  state.staged_registration_table = nullptr;
  state.walked_image_count = 0;
  ClearImageWalkSnapshotUnlocked(state);
}

}  // namespace objc3c::runtime
