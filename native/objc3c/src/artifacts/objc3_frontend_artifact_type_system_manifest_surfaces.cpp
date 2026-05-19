#include "artifacts/objc3_frontend_artifact_type_system_manifest_surfaces.h"

#include <ostream>

#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

namespace objc3::artifacts::frontend {

void WriteTypeSystemManifestSurfaces(
    std::ostream &manifest,
    const Objc3FrontendLightweightGenericsConstraintLoweringContractRecord
        &lightweight_generic_constraint_lowering_contract,
    const std::string &lightweight_generic_constraint_lowering_replay_key,
    const Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord
        &nullability_flow_warning_precision_lowering_contract,
    const std::string &nullability_flow_warning_precision_lowering_replay_key,
    const Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord
        &protocol_qualified_object_type_lowering_contract,
    const std::string &protocol_qualified_object_type_lowering_replay_key,
    const Objc3FrontendVarianceBridgeCastLoweringContractRecord
        &variance_bridge_cast_lowering_contract,
    const std::string &variance_bridge_cast_lowering_replay_key,
    const Objc3FrontendGenericMetadataAbiLoweringContractRecord
        &generic_metadata_abi_lowering_contract,
    const std::string &generic_metadata_abi_lowering_replay_key) {
  manifest
      << ",\"objc_lightweight_generic_constraint_lowering_surface\":{\"generic_constraint_sites\":"
      << lightweight_generic_constraint_lowering_contract.generic_constraint_sites
      << ",\"generic_suffix_sites\":"
      << lightweight_generic_constraint_lowering_contract.generic_suffix_sites
      << ",\"object_pointer_type_sites\":"
      << lightweight_generic_constraint_lowering_contract.object_pointer_type_sites
      << ",\"terminated_generic_suffix_sites\":"
      << lightweight_generic_constraint_lowering_contract
             .terminated_generic_suffix_sites
      << ",\"pointer_declarator_sites\":"
      << lightweight_generic_constraint_lowering_contract.pointer_declarator_sites
      << ",\"normalized_constraint_sites\":"
      << lightweight_generic_constraint_lowering_contract
             .normalized_constraint_sites
      << ",\"contract_violation_sites\":"
      << lightweight_generic_constraint_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << lightweight_generic_constraint_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (lightweight_generic_constraint_lowering_contract.deterministic
              ? "true"
              : "false")
      << "}"
      << ",\"objc_nullability_flow_warning_precision_lowering_surface\":{\"nullability_flow_sites\":"
      << nullability_flow_warning_precision_lowering_contract
             .nullability_flow_sites
      << ",\"object_pointer_type_sites\":"
      << nullability_flow_warning_precision_lowering_contract
             .object_pointer_type_sites
      << ",\"nullability_suffix_sites\":"
      << nullability_flow_warning_precision_lowering_contract
             .nullability_suffix_sites
      << ",\"nullable_suffix_sites\":"
      << nullability_flow_warning_precision_lowering_contract
             .nullable_suffix_sites
      << ",\"nonnull_suffix_sites\":"
      << nullability_flow_warning_precision_lowering_contract
             .nonnull_suffix_sites
      << ",\"normalized_sites\":"
      << nullability_flow_warning_precision_lowering_contract.normalized_sites
      << ",\"contract_violation_sites\":"
      << nullability_flow_warning_precision_lowering_contract
             .contract_violation_sites
      << ",\"replay_key\":\""
      << nullability_flow_warning_precision_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (nullability_flow_warning_precision_lowering_contract.deterministic
              ? "true"
              : "false")
      << "}"
      << ",\"objc_protocol_qualified_object_type_lowering_surface\":{\"protocol_qualified_object_type_sites\":"
      << protocol_qualified_object_type_lowering_contract
             .protocol_qualified_object_type_sites
      << ",\"protocol_composition_sites\":"
      << protocol_qualified_object_type_lowering_contract
             .protocol_composition_sites
      << ",\"object_pointer_type_sites\":"
      << protocol_qualified_object_type_lowering_contract
             .object_pointer_type_sites
      << ",\"terminated_protocol_composition_sites\":"
      << protocol_qualified_object_type_lowering_contract
             .terminated_protocol_composition_sites
      << ",\"pointer_declarator_sites\":"
      << protocol_qualified_object_type_lowering_contract
             .pointer_declarator_sites
      << ",\"normalized_protocol_composition_sites\":"
      << protocol_qualified_object_type_lowering_contract
             .normalized_protocol_composition_sites
      << ",\"contract_violation_sites\":"
      << protocol_qualified_object_type_lowering_contract
             .contract_violation_sites
      << ",\"replay_key\":\""
      << protocol_qualified_object_type_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (protocol_qualified_object_type_lowering_contract.deterministic
              ? "true"
              : "false")
      << "}"
      << ",\"objc_variance_bridge_cast_lowering_surface\":{\"variance_bridge_cast_sites\":"
      << variance_bridge_cast_lowering_contract.variance_bridge_cast_sites
      << ",\"protocol_composition_sites\":"
      << variance_bridge_cast_lowering_contract.protocol_composition_sites
      << ",\"ownership_qualifier_sites\":"
      << variance_bridge_cast_lowering_contract.ownership_qualifier_sites
      << ",\"object_pointer_type_sites\":"
      << variance_bridge_cast_lowering_contract.object_pointer_type_sites
      << ",\"pointer_declarator_sites\":"
      << variance_bridge_cast_lowering_contract.pointer_declarator_sites
      << ",\"normalized_sites\":"
      << variance_bridge_cast_lowering_contract.normalized_sites
      << ",\"contract_violation_sites\":"
      << variance_bridge_cast_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << variance_bridge_cast_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (variance_bridge_cast_lowering_contract.deterministic ? "true"
                                                               : "false")
      << "}"
      << ",\"objc_generic_metadata_abi_lowering_surface\":{\"generic_metadata_abi_sites\":"
      << generic_metadata_abi_lowering_contract.generic_metadata_abi_sites
      << ",\"generic_suffix_sites\":"
      << generic_metadata_abi_lowering_contract.generic_suffix_sites
      << ",\"protocol_composition_sites\":"
      << generic_metadata_abi_lowering_contract.protocol_composition_sites
      << ",\"ownership_qualifier_sites\":"
      << generic_metadata_abi_lowering_contract.ownership_qualifier_sites
      << ",\"object_pointer_type_sites\":"
      << generic_metadata_abi_lowering_contract.object_pointer_type_sites
      << ",\"pointer_declarator_sites\":"
      << generic_metadata_abi_lowering_contract.pointer_declarator_sites
      << ",\"normalized_sites\":"
      << generic_metadata_abi_lowering_contract.normalized_sites
      << ",\"contract_violation_sites\":"
      << generic_metadata_abi_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << generic_metadata_abi_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (generic_metadata_abi_lowering_contract.deterministic ? "true"
                                                               : "false")
      << "}";
}

}  // namespace objc3::artifacts::frontend
