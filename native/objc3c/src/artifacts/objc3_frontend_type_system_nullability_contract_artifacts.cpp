#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

#include <algorithm>
#include <string>

#include "lower/contracts/type_system_generic_lowering_validation_contracts.h"

namespace objc3::artifacts::frontend {

Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord
BuildNullabilityFlowWarningPrecisionLoweringContract(
    const Objc3FrontendTypeSystemParitySurfaceRecord &sema_parity_surface) {
  Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord contract;
  contract.nullability_flow_sites =
      sema_parity_surface.nullability_flow_sites_total;
  contract.object_pointer_type_sites = std::max(
      sema_parity_surface.nullability_flow_object_pointer_type_sites_total,
      sema_parity_surface.nullability_flow_nullability_suffix_sites_total);
  contract.nullability_suffix_sites =
      sema_parity_surface.nullability_flow_nullability_suffix_sites_total;
  contract.nullable_suffix_sites =
      sema_parity_surface.nullability_flow_nullable_suffix_sites_total;
  contract.nonnull_suffix_sites =
      sema_parity_surface.nullability_flow_nonnull_suffix_sites_total;
  contract.normalized_sites =
      sema_parity_surface.nullability_flow_normalized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.nullability_flow_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.nullability_flow_warning_precision_deterministic &&
      sema_parity_surface
          .deterministic_nullability_flow_warning_precision_handoff;
  return contract;
}

bool IsValidFrontendNullabilityFlowWarningPrecisionLoweringContract(
    const Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord
        &contract) {
  return ::IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(
      contract);
}

std::string FrontendNullabilityFlowWarningPrecisionLoweringReplayKey(
    const Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord
        &contract) {
  return ::Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(contract);
}

}  // namespace objc3::artifacts::frontend
