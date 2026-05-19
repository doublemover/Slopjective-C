#include "runtime/images/registration_state_publish.h"

#include "runtime/metadata/runtime_ownership_contracts.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/state/runtime_state_records.h"

#include <cstdint>

namespace objc3c::runtime {

void MarkRejectedRegistrationUnlocked(
    RuntimeState &state, const objc3_runtime_image_descriptor *image,
    int status) {
  state.runtime_owner_split_explicit = RuntimeOwnerSplitContractIsReady();
  state.retired_route_path_allowed = RuntimeRetiredRoutePathsAreAllowed();
  state.last_registration_status = status;
  state.last_rejected_module_name =
      image != nullptr && image->module_name != nullptr ? image->module_name : "";
  state.last_rejected_translation_unit_identity_key =
      image != nullptr && image->translation_unit_identity_key != nullptr
          ? image->translation_unit_identity_key
          : "";
  state.last_rejected_registration_order_ordinal =
      image != nullptr ? image->registration_order_ordinal : 0;
}

void ClearRejectedRegistrationUnlocked(RuntimeState &state) {
  state.last_rejected_module_name.clear();
  state.last_rejected_translation_unit_identity_key.clear();
  state.last_rejected_registration_order_ordinal = 0;
}

void ApplyImageWalkRecordUnlocked(RuntimeState &state,
                                  const RegisteredImageMetadata &record) {
  state.owner_split_contract_id = record.owner_split_contract_id;
  state.metadata_model_owner = record.metadata_model_owner;
  state.registration_table_owner = record.registration_table_owner;
  state.manifest_descriptor_artifact_owner =
      record.manifest_descriptor_artifact_owner;
  state.bootstrap_replay_owner = record.bootstrap_replay_owner;
  state.fail_closed_ownership_model = record.fail_closed_ownership_model;
  state.runtime_owner_split_explicit = record.ownership_explicit;
  state.retired_route_path_allowed = record.retired_route_path_allowed;
  state.walked_image_count = static_cast<std::uint64_t>(
      state.registered_image_metadata_by_identity_key.size());
  state.last_discovery_root_entry_count = record.discovery_root_entry_count;
  state.last_walked_class_descriptor_count = record.class_descriptor_count;
  state.last_walked_protocol_descriptor_count = record.protocol_descriptor_count;
  state.last_walked_category_descriptor_count = record.category_descriptor_count;
  state.last_walked_property_descriptor_count = record.property_descriptor_count;
  state.last_walked_ivar_descriptor_count = record.ivar_descriptor_count;
  state.last_walked_selector_pool_count = record.selector_pool_count;
  state.last_walked_string_pool_count = record.string_pool_count;
  state.last_walked_keypath_descriptor_count = record.keypath_descriptor_count;
  state.last_linker_anchor_matches_discovery_root =
      record.linker_anchor_matches_discovery_root;
  state.last_registration_used_staged_table =
      record.used_staged_registration_table;
  state.last_walked_module_name = record.module_name;
  state.last_walked_translation_unit_identity_key =
      record.translation_unit_identity_key;
}

void RetainBootstrapRecordUnlocked(RuntimeState &state,
                                   const RegisteredImageMetadata &record) {
  const auto found = state.retained_bootstrap_metadata_by_identity_key.find(
      record.translation_unit_identity_key);
  if (found == state.retained_bootstrap_metadata_by_identity_key.end()) {
    state.retained_bootstrap_identity_order.push_back(
        record.translation_unit_identity_key);
  }
  state.retained_bootstrap_metadata_by_identity_key
      [record.translation_unit_identity_key] = record;
}

}  // namespace objc3c::runtime
