#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

#include <algorithm>
#include <cstddef>
#include <string>

#include "artifacts/objc3_frontend_type_system_contract_artifact_replay_fields.h"

namespace objc3::artifacts::frontend {

Objc3FrontendGenericMetadataAbiLoweringContractRecord
BuildGenericMetadataAbiLoweringContract(
    const Objc3FrontendTypeSystemParitySurfaceRecord &sema_parity_surface) {
  Objc3FrontendGenericMetadataAbiLoweringContractRecord contract;
  const std::size_t raw_sites =
      sema_parity_surface.generic_metadata_abi_sites_total;
  const std::size_t raw_generic_suffix_sites =
      sema_parity_surface.generic_metadata_abi_generic_suffix_sites_total;
  const std::size_t raw_protocol_sites =
      sema_parity_surface.generic_metadata_abi_protocol_composition_sites_total;
  const std::size_t raw_ownership_sites =
      sema_parity_surface.generic_metadata_abi_ownership_qualifier_sites_total;
  const std::size_t raw_object_sites =
      sema_parity_surface.generic_metadata_abi_object_pointer_type_sites_total;
  const std::size_t raw_pointer_sites =
      sema_parity_surface.generic_metadata_abi_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.generic_metadata_abi_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.generic_metadata_abi_contract_violation_sites_total;

  contract.generic_metadata_abi_sites =
      std::max({raw_sites, raw_generic_suffix_sites, raw_protocol_sites,
                raw_ownership_sites, raw_pointer_sites, raw_normalized_sites,
                raw_violation_sites});
  contract.generic_suffix_sites =
      std::min(raw_generic_suffix_sites, contract.generic_metadata_abi_sites);
  contract.protocol_composition_sites =
      std::min(raw_protocol_sites, contract.generic_metadata_abi_sites);
  contract.ownership_qualifier_sites =
      std::min(raw_ownership_sites, contract.generic_metadata_abi_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_sites, contract.protocol_composition_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_sites, contract.generic_metadata_abi_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.generic_metadata_abi_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.generic_metadata_abi_sites);

  contract.deterministic =
      sema_parity_surface.generic_metadata_abi_deterministic &&
      sema_parity_surface.deterministic_generic_metadata_abi_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites == contract.generic_metadata_abi_sites;
  return contract;
}

bool IsValidFrontendGenericMetadataAbiLoweringContract(
    const Objc3FrontendGenericMetadataAbiLoweringContractRecord &contract) {
  if (contract.generic_suffix_sites > contract.generic_metadata_abi_sites ||
      contract.protocol_composition_sites >
          contract.generic_metadata_abi_sites ||
      contract.ownership_qualifier_sites >
          contract.generic_metadata_abi_sites ||
      contract.object_pointer_type_sites < contract.protocol_composition_sites ||
      contract.pointer_declarator_sites > contract.generic_metadata_abi_sites ||
      contract.normalized_sites > contract.generic_metadata_abi_sites ||
      contract.contract_violation_sites > contract.generic_metadata_abi_sites) {
    return false;
  }
  return !((contract.contract_violation_sites > 0 ||
            contract.normalized_sites != contract.generic_metadata_abi_sites) &&
           contract.deterministic);
}

std::string FrontendGenericMetadataAbiLoweringReplayKey(
    const Objc3FrontendGenericMetadataAbiLoweringContractRecord &contract) {
  return std::string("generic_metadata_abi_sites=") +
         std::to_string(contract.generic_metadata_abi_sites) +
         ";generic_suffix_sites=" +
         std::to_string(contract.generic_suffix_sites) +
         ";protocol_composition_sites=" +
         std::to_string(contract.protocol_composition_sites) +
         ";ownership_qualifier_sites=" +
         std::to_string(contract.ownership_qualifier_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" +
         type_system_contract_artifacts::BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3FrontendGenericMetadataAbiLoweringLaneContract;
}

}  // namespace objc3::artifacts::frontend
