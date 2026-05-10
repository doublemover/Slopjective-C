#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

#include <algorithm>
#include <cstddef>
#include <string>

#include "artifacts/objc3_frontend_type_system_contract_artifact_replay_fields.h"

namespace objc3::artifacts::frontend {

Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord
BuildProtocolQualifiedObjectTypeLoweringContract(
    const Objc3FrontendTypeSystemParitySurfaceRecord &sema_parity_surface) {
  Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord contract;
  const std::size_t raw_protocol_sites =
      sema_parity_surface.protocol_qualified_object_type_sites_total;
  const std::size_t raw_protocol_composition_sites =
      sema_parity_surface
          .protocol_qualified_object_type_protocol_composition_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface
          .protocol_qualified_object_type_object_pointer_type_sites_total;
  const std::size_t raw_terminated_sites =
      sema_parity_surface
          .protocol_qualified_object_type_terminated_protocol_composition_sites_total;
  const std::size_t raw_pointer_sites =
      sema_parity_surface
          .protocol_qualified_object_type_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface
          .protocol_qualified_object_type_normalized_protocol_composition_sites_total;
  const std::size_t raw_contract_violation_sites =
      sema_parity_surface
          .protocol_qualified_object_type_contract_violation_sites_total;

  contract.protocol_qualified_object_type_sites =
      std::max({raw_protocol_sites, raw_protocol_composition_sites,
                raw_pointer_sites, raw_normalized_sites,
                raw_contract_violation_sites});
  contract.protocol_composition_sites = std::min(
      raw_protocol_composition_sites,
      contract.protocol_qualified_object_type_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.protocol_composition_sites);
  contract.terminated_protocol_composition_sites =
      std::min(raw_terminated_sites, contract.protocol_composition_sites);
  contract.pointer_declarator_sites = std::min(
      raw_pointer_sites, contract.protocol_qualified_object_type_sites);
  contract.normalized_protocol_composition_sites =
      std::min(raw_normalized_sites,
               contract.protocol_qualified_object_type_sites);
  contract.contract_violation_sites =
      std::min(raw_contract_violation_sites,
               contract.protocol_qualified_object_type_sites);

  const bool strict_deterministic =
      sema_parity_surface.protocol_qualified_object_type_deterministic &&
      sema_parity_surface.deterministic_protocol_qualified_object_type_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_protocol_composition_sites ==
          contract.protocol_qualified_object_type_sites;
  contract.deterministic = strict_deterministic;
  return contract;
}

bool IsValidFrontendProtocolQualifiedObjectTypeLoweringContract(
    const Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord
        &contract) {
  if (contract.terminated_protocol_composition_sites >
          contract.protocol_composition_sites ||
      contract.normalized_protocol_composition_sites >
          contract.protocol_qualified_object_type_sites ||
      contract.contract_violation_sites >
          contract.protocol_qualified_object_type_sites) {
    return false;
  }
  return !((contract.contract_violation_sites > 0 ||
            contract.normalized_protocol_composition_sites !=
                contract.protocol_qualified_object_type_sites) &&
           contract.deterministic);
}

std::string FrontendProtocolQualifiedObjectTypeLoweringReplayKey(
    const Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord
        &contract) {
  return std::string("protocol_qualified_object_type_sites=") +
         std::to_string(contract.protocol_qualified_object_type_sites) +
         ";protocol_composition_sites=" +
         std::to_string(contract.protocol_composition_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";terminated_protocol_composition_sites=" +
         std::to_string(contract.terminated_protocol_composition_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_protocol_composition_sites=" +
         std::to_string(contract.normalized_protocol_composition_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" +
         type_system_contract_artifacts::BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3FrontendProtocolQualifiedObjectTypeLoweringLaneContract;
}

}  // namespace objc3::artifacts::frontend
