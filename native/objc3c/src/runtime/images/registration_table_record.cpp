#include "runtime/images/registration_table_record.h"

#include "runtime/metadata/runtime_ownership_contracts.h"
#include "runtime/metadata/runtime_registration_records.h"

namespace objc3c::runtime {

void PublishRegistrationTableRecord(
    RegisteredImageMetadata &record,
    const objc3_runtime_registration_table *registration_table,
    const objc3_runtime_image_descriptor *image,
    const RuntimeRegistrationTableDescriptorCounts &counts,
    bool linker_anchor_matches_discovery_root) {
  record.module_name = image->module_name;
  record.owner_split_contract_id = kObjc3RuntimeOwnerSplitContractId;
  record.metadata_model_owner = kObjc3RuntimeMetadataModelOwner;
  record.registration_table_owner = kObjc3RuntimeRegistrationTableOwner;
  record.manifest_descriptor_artifact_owner =
      kObjc3RuntimeManifestDescriptorArtifactOwner;
  record.bootstrap_replay_owner = kObjc3RuntimeBootstrapReplayOwner;
  record.fail_closed_ownership_model = kObjc3RuntimeFailClosedOwnershipModel;
  record.translation_unit_identity_key = image->translation_unit_identity_key;
  record.registration_order_ordinal = image->registration_order_ordinal;
  record.registration_table = registration_table;
  record.discovery_root = registration_table->discovery_root;
  record.class_descriptor_root = registration_table->class_descriptor_root;
  record.protocol_descriptor_root = registration_table->protocol_descriptor_root;
  record.category_descriptor_root = registration_table->category_descriptor_root;
  record.property_descriptor_root = registration_table->property_descriptor_root;
  record.ivar_descriptor_root = registration_table->ivar_descriptor_root;
  record.selector_pool_root = registration_table->selector_pool_root;
  record.string_pool_root = registration_table->string_pool_root;
  record.keypath_descriptor_root = registration_table->keypath_descriptor_root;
  record.discovery_root_entry_count = counts.discovery_root_entry_count;
  record.class_descriptor_count = counts.class_descriptor_count;
  record.protocol_descriptor_count = counts.protocol_descriptor_count;
  record.category_descriptor_count = counts.category_descriptor_count;
  record.property_descriptor_count = counts.property_descriptor_count;
  record.ivar_descriptor_count = counts.ivar_descriptor_count;
  record.selector_pool_count = counts.selector_pool_count;
  record.string_pool_count = counts.string_pool_count;
  record.keypath_descriptor_count = counts.keypath_descriptor_count;
  record.linker_anchor_matches_discovery_root =
      linker_anchor_matches_discovery_root;
  record.used_staged_registration_table = true;
  record.ownership_explicit = RuntimeOwnerSplitContractIsReady();
  record.retired_route_path_allowed = RuntimeRetiredRoutePathsAreAllowed();
}

}  // namespace objc3c::runtime
