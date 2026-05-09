#include "io/objc3_runtime_registration_descriptor_document.h"

#include <sstream>

#include "io/objc3_process_internal.h"
#include "runtime/metadata/runtime_ownership_contracts.h"

std::string BuildObjc3RuntimeRegistrationDescriptorArtifactDocumentJson(
    const Objc3RuntimeRegistrationDescriptorArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &linker_retention_artifacts,
    const std::string &constructor_init_stub_symbol,
    const std::string &bootstrap_registration_table_symbol,
    const std::string &bootstrap_image_local_init_state_symbol) {
  std::ostringstream out;
  JsonObjectWriter descriptor(out);
  descriptor.StringField("contract_id", inputs.contract_id);
  descriptor.StringField("owner_split_contract_id",
                         objc3c::runtime::kObjc3RuntimeOwnerSplitContractId);
  descriptor.StringField("metadata_model_owner",
                         objc3c::runtime::kObjc3RuntimeMetadataModelOwner);
  descriptor.StringField("registration_table_owner",
                         objc3c::runtime::kObjc3RuntimeRegistrationTableOwner);
  descriptor.StringField(
      "manifest_descriptor_artifact_owner",
      objc3c::runtime::kObjc3RuntimeManifestDescriptorArtifactOwner);
  descriptor.StringField("bootstrap_replay_owner",
                         objc3c::runtime::kObjc3RuntimeBootstrapReplayOwner);
  descriptor.StringField(
      "dispatch_frame_state_owner",
      objc3c::runtime::kObjc3RuntimeDispatchFrameStateOwner);
  descriptor.StringField(
      "public_registration_api_owner",
      objc3c::runtime::kObjc3RuntimePublicRegistrationApiOwner);
  descriptor.StringField(
      "public_dispatch_diagnostics_owner",
      objc3c::runtime::kObjc3RuntimePublicDispatchDiagnosticsOwner);
  descriptor.StringField(
      "fail_closed_ownership_model",
      objc3c::runtime::kObjc3RuntimeFailClosedOwnershipModel);
  descriptor.BoolField("fallback_path_allowed", false);
  descriptor.StringField("registration_manifest_contract_id",
                         inputs.registration_manifest_contract_id);
  descriptor.StringField("source_surface_contract_id",
                         inputs.source_surface_contract_id);
  descriptor.StringField("bootstrap_lowering_contract_id",
                         inputs.bootstrap_lowering_contract_id);
  descriptor.StringField("payload_model", inputs.payload_model);
  descriptor.StringField("artifact", inputs.artifact_relative_path);
  descriptor.StringField("authority_model", inputs.authority_model);
  descriptor.StringField("translation_unit_identity_model",
                         inputs.translation_unit_identity_model);
  descriptor.StringField(
      "translation_unit_identity_key",
      linker_retention_artifacts.translation_unit_identity_key);
  descriptor.StringField("payload_ownership_model",
                         inputs.payload_ownership_model);
  descriptor.StringField("runtime_support_library_archive_relative_path",
                         inputs.runtime_support_library_archive_relative_path);
  descriptor.StringField("registration_entrypoint_symbol",
                         inputs.registration_entrypoint_symbol);
  descriptor.StringField("registration_descriptor_pragma_name",
                         inputs.registration_descriptor_pragma_name);
  descriptor.StringField("image_root_pragma_name",
                         inputs.image_root_pragma_name);
  descriptor.StringField("module_identity_source",
                         inputs.module_identity_source);
  descriptor.StringField("registration_descriptor_identifier",
                         inputs.registration_descriptor_identifier);
  descriptor.StringField("registration_descriptor_identity_source",
                         inputs.registration_descriptor_identity_source);
  descriptor.StringField("image_root_identifier", inputs.image_root_identifier);
  descriptor.StringField("image_root_identity_source",
                         inputs.image_root_identity_source);
  descriptor.StringField("bootstrap_visible_metadata_ownership_model",
                         inputs.bootstrap_visible_metadata_ownership_model);
  descriptor.StringField("constructor_root_symbol",
                         inputs.constructor_root_symbol);
  descriptor.StringField("constructor_init_stub_symbol",
                         constructor_init_stub_symbol);
  descriptor.StringField("bootstrap_registration_table_symbol",
                         bootstrap_registration_table_symbol);
  descriptor.StringField("bootstrap_image_local_init_state_symbol",
                         bootstrap_image_local_init_state_symbol);
  descriptor.StringField("bootstrap_registration_table_layout_model",
                         inputs.bootstrap_registration_table_layout_model);
  descriptor.StringField("bootstrap_image_local_initialization_model",
                         inputs.bootstrap_image_local_initialization_model);
  descriptor.StringField("bootstrap_constructor_root_emission_state",
                         inputs.bootstrap_constructor_root_emission_state);
  descriptor.StringField("bootstrap_init_stub_emission_state",
                         inputs.bootstrap_init_stub_emission_state);
  descriptor.StringField("bootstrap_registration_table_emission_state",
                         inputs.bootstrap_registration_table_emission_state);
  descriptor.UnsignedField("bootstrap_registration_table_abi_version",
                           inputs.bootstrap_registration_table_abi_version);
  descriptor.UnsignedField(
      "bootstrap_registration_table_pointer_field_count",
      inputs.bootstrap_registration_table_pointer_field_count);
  descriptor.SizeField("class_descriptor_count",
                       inputs.class_descriptor_count);
  descriptor.SizeField("protocol_descriptor_count",
                       inputs.protocol_descriptor_count);
  descriptor.SizeField("category_descriptor_count",
                       inputs.category_descriptor_count);
  descriptor.SizeField("property_descriptor_count",
                       inputs.property_descriptor_count);
  descriptor.SizeField("ivar_descriptor_count", inputs.ivar_descriptor_count);
  descriptor.SizeField("total_descriptor_count", inputs.total_descriptor_count);
  descriptor.UnsignedField("translation_unit_registration_order_ordinal",
                           inputs.translation_unit_registration_order_ordinal);
  descriptor.StringField("object_artifact",
                         inputs.object_artifact_relative_path);
  descriptor.StringField("backend_artifact",
                         inputs.backend_artifact_relative_path);
  descriptor.StringField("object_format",
                         linker_retention_artifacts.object_format);
  descriptor.StringField("linker_anchor_symbol",
                         linker_retention_artifacts.linker_anchor_symbol);
  descriptor.StringField("discovery_root_symbol",
                         linker_retention_artifacts.discovery_root_symbol);
  descriptor.BoolField("ready_for_descriptor_artifact_emission", true);
  descriptor.BoolField("ready_for_registration_descriptor_lowering", true);
  descriptor.BoolField("ready_for_loader_table_lowering", true);
  return FinishJsonObject(descriptor, out);
}
