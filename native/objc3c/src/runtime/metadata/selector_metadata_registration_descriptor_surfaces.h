#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

struct Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary {
  std::string contract_id =
      kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceContractId;
  std::string registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string source_surface_path =
      kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfacePath;
  std::string registration_descriptor_pragma_name =
      kObjc3BootstrapRegistrationDescriptorPragmaName;
  std::string image_root_pragma_name = kObjc3BootstrapImageRootPragmaName;
  std::string module_identity_source =
      kObjc3RuntimeBootstrapModuleIdentitySourceModel;
  std::string registration_descriptor_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string image_root_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string bootstrap_visible_metadata_ownership_model =
      kObjc3RuntimeBootstrapVisibleMetadataOwnershipModel;
  std::string module_name = "objc3_module";
  std::string registration_descriptor_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix;
  std::string image_root_identifier =
      std::string("objc3_module") + kObjc3RuntimeBootstrapImageRootDefaultSuffix;
  bool fail_closed = false;
  bool registration_manifest_contract_ready = false;
  bool source_surface_frozen = false;
  bool prelude_pragma_contract_published = false;
  bool registration_descriptor_identifier_resolved = false;
  bool image_root_identifier_resolved = false;
  bool bootstrap_visible_metadata_ownership_published = false;
  bool ready_for_descriptor_frontend_closure = false;
  bool registration_descriptor_pragma_seen = false;
  bool registration_descriptor_pragma_duplicate = false;
  bool registration_descriptor_pragma_non_leading = false;
  std::size_t registration_descriptor_pragma_directive_count = 0;
  bool image_root_pragma_seen = false;
  bool image_root_pragma_duplicate = false;
  bool image_root_pragma_non_leading = false;
  std::size_t image_root_pragma_directive_count = 0;
  std::string registration_manifest_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &summary) {
  return !summary.contract_id.empty() &&
         !summary.registration_manifest_contract_id.empty() &&
         !summary.source_surface_path.empty() &&
         !summary.registration_descriptor_pragma_name.empty() &&
         !summary.image_root_pragma_name.empty() &&
         !summary.module_identity_source.empty() &&
         !summary.registration_descriptor_identity_source.empty() &&
         !summary.image_root_identity_source.empty() &&
         !summary.bootstrap_visible_metadata_ownership_model.empty() &&
         !summary.module_name.empty() &&
         !summary.registration_descriptor_identifier.empty() &&
         !summary.image_root_identifier.empty() &&
         summary.fail_closed &&
         summary.registration_manifest_contract_ready &&
         summary.source_surface_frozen &&
         summary.prelude_pragma_contract_published &&
         summary.registration_descriptor_identifier_resolved &&
         summary.image_root_identifier_resolved &&
         summary.bootstrap_visible_metadata_ownership_published &&
         summary.ready_for_descriptor_frontend_closure &&
         !summary.registration_manifest_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3RuntimeRegistrationDescriptorFrontendClosureSummary {
  std::string contract_id =
      kObjc3RuntimeRegistrationDescriptorFrontendClosureContractId;
  std::string registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string source_surface_contract_id =
      kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceContractId;
  std::string frontend_surface_path =
      kObjc3RuntimeRegistrationDescriptorFrontendClosureSurfacePath;
  std::string payload_model =
      kObjc3RuntimeRegistrationDescriptorFrontendClosurePayloadModel;
  std::string artifact_relative_path =
      kObjc3RuntimeRegistrationDescriptorFrontendClosureArtifactRelativePath;
  std::string authority_model =
      kObjc3RuntimeRegistrationDescriptorFrontendAuthorityModel;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  std::string payload_ownership_model =
      kObjc3RuntimeRegistrationDescriptorPayloadOwnershipModel;
  std::string registration_descriptor_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix;
  std::string image_root_identifier =
      std::string("objc3_module") +
      kObjc3RuntimeBootstrapImageRootDefaultSuffix;
  std::string registration_descriptor_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string image_root_identity_source =
      kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault;
  std::string bootstrap_visible_metadata_ownership_model =
      kObjc3RuntimeBootstrapVisibleMetadataOwnershipModel;
  std::size_t class_descriptor_count = 0;
  std::size_t protocol_descriptor_count = 0;
  std::size_t category_descriptor_count = 0;
  std::size_t property_descriptor_count = 0;
  std::size_t ivar_descriptor_count = 0;
  std::size_t total_descriptor_count = 0;
  std::uint64_t translation_unit_registration_order_ordinal =
      kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal;
  bool fail_closed = false;
  bool source_surface_contract_ready = false;
  bool registration_manifest_contract_ready = false;
  bool descriptor_frontend_surface_published = false;
  bool descriptor_artifact_template_published = false;
  bool descriptor_fields_resolved = false;
  bool ready_for_descriptor_artifact_emission = false;
  bool ready_for_registration_descriptor_lowering = false;
  std::string source_surface_replay_key;
  std::string registration_manifest_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.registration_manifest_contract_id.empty() &&
         !summary.source_surface_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.payload_model.empty() &&
         !summary.artifact_relative_path.empty() &&
         !summary.authority_model.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.payload_ownership_model.empty() &&
         !summary.registration_descriptor_identifier.empty() &&
         !summary.image_root_identifier.empty() &&
         !summary.registration_descriptor_identity_source.empty() &&
         !summary.image_root_identity_source.empty() &&
         !summary.bootstrap_visible_metadata_ownership_model.empty() &&
         summary.total_descriptor_count ==
             summary.class_descriptor_count +
                 summary.protocol_descriptor_count +
                 summary.category_descriptor_count +
                 summary.property_descriptor_count +
                 summary.ivar_descriptor_count &&
         summary.translation_unit_registration_order_ordinal > 0 &&
         summary.fail_closed && summary.source_surface_contract_ready &&
         summary.registration_manifest_contract_ready &&
         summary.descriptor_frontend_surface_published &&
         summary.descriptor_artifact_template_published &&
         summary.descriptor_fields_resolved &&
         summary.ready_for_descriptor_artifact_emission &&
         summary.ready_for_registration_descriptor_lowering &&
         !summary.source_surface_replay_key.empty() &&
         !summary.registration_manifest_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
