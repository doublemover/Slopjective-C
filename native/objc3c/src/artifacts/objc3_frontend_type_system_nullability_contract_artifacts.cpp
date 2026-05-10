#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

#include <algorithm>
#include <string>

#include "artifacts/objc3_frontend_type_system_contract_artifact_replay_fields.h"

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
  if (contract.nullability_suffix_sites > contract.nullability_flow_sites ||
      contract.nullable_suffix_sites > contract.nullability_suffix_sites ||
      contract.nonnull_suffix_sites > contract.nullability_suffix_sites ||
      contract.object_pointer_type_sites < contract.nullability_suffix_sites ||
      contract.normalized_sites > contract.nullability_flow_sites ||
      contract.contract_violation_sites > contract.nullability_flow_sites) {
    return false;
  }
  if (contract.nullability_suffix_sites !=
      contract.nullable_suffix_sites + contract.nonnull_suffix_sites) {
    return false;
  }
  return !((contract.contract_violation_sites > 0 ||
            contract.normalized_sites != contract.nullability_flow_sites) &&
           contract.deterministic);
}

std::string FrontendNullabilityFlowWarningPrecisionLoweringReplayKey(
    const Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord
        &contract) {
  return std::string("nullability_flow_sites=") +
         std::to_string(contract.nullability_flow_sites) +
         ";object_pointer_type_sites=" +
         std::to_string(contract.object_pointer_type_sites) +
         ";nullability_suffix_sites=" +
         std::to_string(contract.nullability_suffix_sites) +
         ";nullable_suffix_sites=" +
         std::to_string(contract.nullable_suffix_sites) +
         ";nonnull_suffix_sites=" +
         std::to_string(contract.nonnull_suffix_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" +
         type_system_contract_artifacts::BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3FrontendNullabilityFlowWarningPrecisionLoweringLaneContract;
}

}  // namespace objc3::artifacts::frontend
