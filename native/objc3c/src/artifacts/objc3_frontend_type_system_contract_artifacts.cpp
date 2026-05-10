#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

#include <algorithm>
#include <cstddef>
#include <string>

namespace objc3::artifacts::frontend {
namespace {

std::string BoolToken(bool value) { return value ? "true" : "false"; }

}  // namespace

Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord
BuildFrontendTypeSystemOptionalKeypathLoweringContract(
    const Objc3FrontendTypeSystemSemanticModelRecord &summary) {
  Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord contract;
  contract.optional_binding_sites = summary.optional_binding_sites;
  contract.optional_binding_clause_sites =
      summary.optional_binding_clause_sites;
  contract.optional_send_sites = summary.optional_send_sites;
  contract.nil_coalescing_sites = summary.nil_coalescing_sites;
  contract.typed_keypath_literal_sites = summary.typed_keypath_literal_sites;
  contract.typed_keypath_self_root_sites =
      summary.typed_keypath_self_root_sites;
  contract.typed_keypath_class_root_sites =
      summary.typed_keypath_class_root_sites;
  contract.live_optional_lowering_sites =
      summary.optional_binding_sites + summary.optional_send_sites +
      summary.nil_coalescing_sites;
  contract.single_evaluation_nil_short_circuit_sites =
      summary.optional_binding_clause_sites + summary.optional_send_sites +
      summary.nil_coalescing_sites;
  contract.live_typed_keypath_artifact_sites =
      summary.typed_keypath_literal_sites;
  contract.deferred_typed_keypath_sites = 0;
  contract.contract_violation_sites =
      summary.optional_binding_contract_violation_sites +
      summary.optional_send_contract_violation_sites +
      summary.optional_flow_contract_violation_sites;
  contract.deterministic =
      summary.deterministic && contract.contract_violation_sites == 0 &&
      contract.live_typed_keypath_artifact_sites +
              contract.deferred_typed_keypath_sites ==
          contract.typed_keypath_literal_sites;
  return contract;
}

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

bool IsValidFrontendTypeSystemOptionalKeypathLoweringContract(
    const Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord
        &contract) {
  if (contract.optional_binding_clause_sites > contract.optional_binding_sites ||
      contract.typed_keypath_self_root_sites >
          contract.typed_keypath_literal_sites ||
      contract.typed_keypath_class_root_sites >
          contract.typed_keypath_literal_sites) {
    return false;
  }
  if (contract.live_optional_lowering_sites !=
      contract.optional_binding_sites + contract.optional_send_sites +
          contract.nil_coalescing_sites) {
    return false;
  }
  if (contract.single_evaluation_nil_short_circuit_sites !=
      contract.optional_binding_clause_sites + contract.optional_send_sites +
          contract.nil_coalescing_sites) {
    return false;
  }
  if (contract.live_typed_keypath_artifact_sites +
          contract.deferred_typed_keypath_sites !=
      contract.typed_keypath_literal_sites) {
    return false;
  }
  if (contract.contract_violation_sites >
      contract.live_optional_lowering_sites +
          contract.live_typed_keypath_artifact_sites +
          contract.deferred_typed_keypath_sites) {
    return false;
  }
  return contract.contract_violation_sites == 0 || !contract.deterministic;
}

std::string FrontendTypeSystemOptionalKeypathLoweringReplayKey(
    const Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord
        &contract) {
  return std::string("optional_binding_sites=") +
         std::to_string(contract.optional_binding_sites) +
         ";optional_binding_clause_sites=" +
         std::to_string(contract.optional_binding_clause_sites) +
         ";optional_send_sites=" +
         std::to_string(contract.optional_send_sites) +
         ";nil_coalescing_sites=" +
         std::to_string(contract.nil_coalescing_sites) +
         ";typed_keypath_literal_sites=" +
         std::to_string(contract.typed_keypath_literal_sites) +
         ";typed_keypath_self_root_sites=" +
         std::to_string(contract.typed_keypath_self_root_sites) +
         ";typed_keypath_class_root_sites=" +
         std::to_string(contract.typed_keypath_class_root_sites) +
         ";live_optional_lowering_sites=" +
         std::to_string(contract.live_optional_lowering_sites) +
         ";single_evaluation_nil_short_circuit_sites=" +
         std::to_string(contract.single_evaluation_nil_short_circuit_sites) +
         ";live_typed_keypath_artifact_sites=" +
         std::to_string(contract.live_typed_keypath_artifact_sites) +
         ";deferred_typed_keypath_sites=" +
         std::to_string(contract.deferred_typed_keypath_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3FrontendTypeSystemOptionalKeypathLoweringLaneContract;
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
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3FrontendLightweightGenericsConstraintLoweringLaneContract;
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
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3FrontendNullabilityFlowWarningPrecisionLoweringLaneContract;
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
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3FrontendProtocolQualifiedObjectTypeLoweringLaneContract;
}

bool IsValidFrontendVarianceBridgeCastLoweringContract(
    const Objc3FrontendVarianceBridgeCastLoweringContractRecord &contract) {
  if (contract.protocol_composition_sites >
          contract.variance_bridge_cast_sites ||
      contract.ownership_qualifier_sites >
          contract.variance_bridge_cast_sites ||
      contract.object_pointer_type_sites < contract.protocol_composition_sites ||
      contract.pointer_declarator_sites > contract.variance_bridge_cast_sites ||
      contract.normalized_sites > contract.variance_bridge_cast_sites ||
      contract.contract_violation_sites > contract.variance_bridge_cast_sites) {
    return false;
  }
  return !((contract.contract_violation_sites > 0 ||
            contract.normalized_sites != contract.variance_bridge_cast_sites) &&
           contract.deterministic);
}

std::string FrontendVarianceBridgeCastLoweringReplayKey(
    const Objc3FrontendVarianceBridgeCastLoweringContractRecord &contract) {
  return std::string("variance_bridge_cast_sites=") +
         std::to_string(contract.variance_bridge_cast_sites) +
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
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3FrontendVarianceBridgeCastLoweringLaneContract;
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
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3FrontendGenericMetadataAbiLoweringLaneContract;
}

}  // namespace objc3::artifacts::frontend
