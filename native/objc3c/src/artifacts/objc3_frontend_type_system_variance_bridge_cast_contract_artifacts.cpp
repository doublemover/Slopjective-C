#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

#include <algorithm>
#include <cstddef>
#include <string>

#include "lower/contracts/type_system_generic_lowering_validation_contracts.h"

namespace objc3::artifacts::frontend {

Objc3FrontendVarianceBridgeCastLoweringContractRecord
BuildVarianceBridgeCastLoweringContract(
    const Objc3FrontendTypeSystemParitySurfaceRecord &sema_parity_surface) {
  Objc3FrontendVarianceBridgeCastLoweringContractRecord contract;
  const std::size_t raw_sites =
      sema_parity_surface.variance_bridge_cast_sites_total;
  const std::size_t raw_protocol_sites =
      sema_parity_surface
          .variance_bridge_cast_protocol_composition_sites_total;
  const std::size_t raw_ownership_sites =
      sema_parity_surface.variance_bridge_cast_ownership_qualifier_sites_total;
  const std::size_t raw_object_sites =
      sema_parity_surface.variance_bridge_cast_object_pointer_type_sites_total;
  const std::size_t raw_pointer_sites =
      sema_parity_surface.variance_bridge_cast_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.variance_bridge_cast_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.variance_bridge_cast_contract_violation_sites_total;

  contract.variance_bridge_cast_sites =
      std::max({raw_sites, raw_protocol_sites, raw_ownership_sites,
                raw_pointer_sites, raw_normalized_sites, raw_violation_sites});
  contract.protocol_composition_sites =
      std::min(raw_protocol_sites, contract.variance_bridge_cast_sites);
  contract.ownership_qualifier_sites =
      std::min(raw_ownership_sites, contract.variance_bridge_cast_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_sites, contract.protocol_composition_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_sites, contract.variance_bridge_cast_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.variance_bridge_cast_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.variance_bridge_cast_sites);

  contract.deterministic =
      sema_parity_surface.variance_bridge_cast_deterministic &&
      sema_parity_surface.deterministic_variance_bridge_cast_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites == contract.variance_bridge_cast_sites;
  return contract;
}

bool IsValidFrontendVarianceBridgeCastLoweringContract(
    const Objc3FrontendVarianceBridgeCastLoweringContractRecord &contract) {
  return ::IsValidObjc3VarianceBridgeCastLoweringContract(contract);
}

std::string FrontendVarianceBridgeCastLoweringReplayKey(
    const Objc3FrontendVarianceBridgeCastLoweringContractRecord &contract) {
  return ::Objc3VarianceBridgeCastLoweringReplayKey(contract);
}

}  // namespace objc3::artifacts::frontend
