#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

#include <algorithm>
#include <cstddef>
#include <string>

#include "artifacts/objc3_frontend_type_system_contract_artifact_replay_fields.h"

namespace objc3::artifacts::frontend {

Objc3FrontendLightweightGenericsConstraintLoweringContractRecord
BuildLightweightGenericsConstraintLoweringContract(
    const Objc3FrontendTypeSystemParitySurfaceRecord &sema_parity_surface) {
  Objc3FrontendLightweightGenericsConstraintLoweringContractRecord contract;
  const std::size_t raw_sites =
      sema_parity_surface.lightweight_generic_constraint_sites_total;
  const std::size_t raw_generic_suffix_sites =
      sema_parity_surface
          .lightweight_generic_constraint_generic_suffix_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface
          .lightweight_generic_constraint_object_pointer_type_sites_total;
  const std::size_t raw_terminated_generic_suffix_sites =
      sema_parity_surface
          .lightweight_generic_constraint_terminated_generic_suffix_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface
          .lightweight_generic_constraint_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.lightweight_generic_constraint_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface
          .lightweight_generic_constraint_contract_violation_sites_total;

  contract.generic_constraint_sites =
      std::max({raw_sites, raw_generic_suffix_sites, raw_object_pointer_sites,
                raw_pointer_declarator_sites, raw_normalized_sites,
                raw_violation_sites});
  contract.generic_suffix_sites =
      std::min(raw_generic_suffix_sites, contract.generic_constraint_sites);
  contract.object_pointer_type_sites =
      std::min(raw_object_pointer_sites, contract.generic_constraint_sites);
  contract.terminated_generic_suffix_sites = std::min(
      raw_terminated_generic_suffix_sites, contract.generic_suffix_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.generic_constraint_sites);
  contract.normalized_constraint_sites =
      std::min(raw_normalized_sites, contract.generic_constraint_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.generic_constraint_sites);

  contract.deterministic =
      sema_parity_surface.lightweight_generic_constraint_deterministic &&
      sema_parity_surface.deterministic_lightweight_generic_constraint_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_constraint_sites ==
          contract.generic_constraint_sites;
  return contract;
}

bool IsValidFrontendLightweightGenericsConstraintLoweringContract(
    const Objc3FrontendLightweightGenericsConstraintLoweringContractRecord
        &contract) {
  if (contract.generic_suffix_sites > contract.generic_constraint_sites ||
      contract.object_pointer_type_sites > contract.generic_constraint_sites ||
      contract.terminated_generic_suffix_sites > contract.generic_suffix_sites ||
      contract.pointer_declarator_sites > contract.generic_constraint_sites ||
      contract.normalized_constraint_sites > contract.generic_constraint_sites ||
      contract.contract_violation_sites > contract.generic_constraint_sites) {
    return false;
  }
  return !((contract.contract_violation_sites > 0 ||
            contract.normalized_constraint_sites !=
                contract.generic_constraint_sites) &&
           contract.deterministic);
}

std::string FrontendLightweightGenericsConstraintLoweringReplayKey(
    const Objc3FrontendLightweightGenericsConstraintLoweringContractRecord
        &contract) {
  return std::string("generic_constraint_sites=") +
         std::to_string(contract.generic_constraint_sites) +
         ";generic_suffix_sites=" +
         std::to_string(contract.generic_suffix_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";terminated_generic_suffix_sites=" +
         std::to_string(contract.terminated_generic_suffix_sites) +
         ";pointer_declarator_sites=" +
         std::to_string(contract.pointer_declarator_sites) +
         ";normalized_constraint_sites=" +
         std::to_string(contract.normalized_constraint_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" +
         type_system_contract_artifacts::BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3FrontendLightweightGenericsConstraintLoweringLaneContract;
}

}  // namespace objc3::artifacts::frontend
