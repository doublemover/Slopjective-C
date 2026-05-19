#pragma once

#include <array>
#include <cstddef>
#include <string>

#include "ast/objc3_ast_contracts_metadata_packaging.h"
#include "ast/objc3_ast_contracts_source_property_metadata.h"
#include "contracts/objc3_runtime_metadata_emission_contract_ids.h"
#include "lower/contracts/runtime_bootstrap_link_discovery_contracts.h"
#include "runtime/metadata/executable_metadata_runtime_ingest.h"
#include "runtime/metadata/runtime_support_library_metadata.h"

struct Objc3RuntimeTranslationUnitRegistrationContractSummary {
  std::string contract_id =
      kObjc3RuntimeTranslationUnitRegistrationContractId;
  std::string binary_boundary_contract_id =
      kObjc3ExecutableMetadataRuntimeIngestBinaryBoundaryContractId;
  std::string archive_static_link_contract_id =
      kObjc3RuntimeArchiveStaticLinkDiscoveryContractId;
  std::string object_emission_closeout_contract_id =
      objc3c::contracts::kObjc3RuntimeMetadataObjectEmissionCloseoutContractId;
  std::string runtime_support_library_link_wiring_contract_id =
      kObjc3RuntimeSupportLibraryLinkWiringContractId;
  std::string registration_surface_path =
      kObjc3RuntimeTranslationUnitRegistrationSurfacePath;
  std::string registration_payload_model =
      kObjc3RuntimeTranslationUnitRegistrationPayloadModel;
  std::array<std::string, 3u> runtime_owned_payload_artifacts = {
      kObjc3RuntimeTranslationUnitRegistrationPayloadArtifactRelativePath,
      kObjc3RuntimeTranslationUnitRegistrationLinkerResponseArtifactRelativePath,
      kObjc3RuntimeTranslationUnitRegistrationDiscoveryArtifactRelativePath};
  std::string constructor_root_symbol =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol;
  std::string constructor_root_ownership_model =
      kObjc3RuntimeTranslationUnitRegistrationConstructorRootOwnershipModel;
  std::string constructor_emission_mode =
      kObjc3RuntimeTranslationUnitRegistrationConstructorEmissionMode;
  std::string constructor_priority_policy =
      kObjc3RuntimeTranslationUnitRegistrationConstructorPriorityPolicy;
  std::string registration_entrypoint_symbol =
      kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol;
  std::string translation_unit_identity_model =
      kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel;
  bool boundary_frozen = false;
  bool fail_closed = false;
  bool binary_boundary_ready = false;
  bool archive_static_link_surface_ready = false;
  bool object_emission_closeout_surface_ready = false;
  bool runtime_support_library_link_wiring_ready = false;
  bool runtime_owned_payload_inventory_published = false;
  bool constructor_root_reserved_not_emitted = false;
  bool startup_registration_not_yet_landed = false;
  bool runtime_bootstrap_not_yet_landed = false;
  bool explicit_non_goals_published = false;
  bool ready_for_registration_manifest_implementation = false;
  std::size_t runtime_owned_payload_artifact_count = 0;
  std::string binary_boundary_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.binary_boundary_contract_id.empty() &&
         !summary.archive_static_link_contract_id.empty() &&
         !summary.object_emission_closeout_contract_id.empty() &&
         !summary.runtime_support_library_link_wiring_contract_id.empty() &&
         !summary.registration_surface_path.empty() &&
         !summary.registration_payload_model.empty() &&
         summary.boundary_frozen &&
         summary.fail_closed &&
         summary.binary_boundary_ready &&
         summary.archive_static_link_surface_ready &&
         summary.object_emission_closeout_surface_ready &&
         summary.runtime_support_library_link_wiring_ready &&
         summary.runtime_owned_payload_inventory_published &&
         summary.constructor_root_reserved_not_emitted &&
         summary.startup_registration_not_yet_landed &&
         summary.runtime_bootstrap_not_yet_landed &&
         summary.explicit_non_goals_published &&
         summary.ready_for_registration_manifest_implementation &&
         summary.runtime_owned_payload_artifact_count ==
             summary.runtime_owned_payload_artifacts.size() &&
         !summary.constructor_root_symbol.empty() &&
         !summary.constructor_root_ownership_model.empty() &&
         !summary.constructor_emission_mode.empty() &&
         !summary.constructor_priority_policy.empty() &&
         !summary.registration_entrypoint_symbol.empty() &&
         !summary.translation_unit_identity_model.empty() &&
         !summary.binary_boundary_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
