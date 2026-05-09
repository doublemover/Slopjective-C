#include "lower/contracts/type_system_lowering_contracts.h"

#include "lower/contracts/type_system_generic_lowering_validation_contracts.h"
#include "lower/metadata/lowering_metadata_helpers.h"

#include <sstream>
#include <string>

bool IsValidObjc3TypeSystemOptionalKeypathLoweringContract(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract) {
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
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3TypeSystemOptionalKeypathLoweringReplayKey(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract) {
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
         kObjc3TypeSystemOptionalKeypathLoweringLaneContract;
}

std::string Objc3TypeSystemOptionalKeypathLoweringSummary() {
  std::ostringstream out;
  // optional chaining lowering anchor: `?.member` now desugars onto
  // the same optional-send ABI and nil-short-circuit path already used by
  // bracketed optional sends, so the live lowering packet truthfully covers
  // optional-member access. The later lowering step now widens the same packet
  // to cover validated typed key-path descriptor emission and stable runtime
  // handles without claiming full key-path application/runtime evaluation yet.
  out << "contract_id=" << kObjc3TypeSystemOptionalKeypathLoweringContractId
      << ";optional_model="
      << kObjc3TypeSystemOptionalKeypathLoweringOptionalModel
      << ";typed_keypath_model="
      << kObjc3TypeSystemOptionalKeypathLoweringTypedKeypathModel
      << ";authority_model="
      << kObjc3TypeSystemOptionalKeypathLoweringAuthorityModel
      << ";fail_closed_model="
      << kObjc3TypeSystemOptionalKeypathLoweringFailClosedModel
      << ";lane_contract="
      << kObjc3TypeSystemOptionalKeypathLoweringLaneContract;
  return out.str();
}

std::string Objc3TypeSystemOptionalKeypathRuntimeHelperContractSummary() {
  std::ostringstream out;
  // live-optional-send-and-keypath-runtime-support anchor: optional
  // sends stay on the public selector lookup/dispatch ABI while validated
  // single-component typed key-path handles now feed the private runtime
  // registry/testing helper surface without falsely claiming full
  // multi-component key-path evaluation.
  out << "contract_id=" << kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId
      << ";surface_path=" << kObjc3TypeSystemOptionalKeypathRuntimeHelperSurfacePath
      << ";optional_model="
      << kObjc3TypeSystemOptionalKeypathRuntimeHelperOptionalModel
      << ";typed_keypath_model="
      << kObjc3TypeSystemOptionalKeypathRuntimeHelperTypedKeypathModel
      << ";diagnostic_model="
      << kObjc3TypeSystemOptionalKeypathRuntimeHelperDiagnosticModel
      << ";lookup_selector_symbol="
      << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
      << ";dispatch_i32_symbol="
      << kObjc3RuntimeSupportLibraryDispatchI32Symbol
      << ";keypath_descriptor_section="
      << kObjc3RuntimeKeypathDescriptorLogicalSection
      << ";keypath_descriptor_aggregate=__objc3_sec_keypath_descriptors"
      << ";typed_keypath_runtime_execution_helper_landed=true";
  return out.str();
}

bool IsValidObjc3LightweightGenericsConstraintLoweringContract(
    const Objc3LightweightGenericsConstraintLoweringContract &contract) {
  if (contract.generic_suffix_sites > contract.generic_constraint_sites ||
      contract.object_pointer_type_sites > contract.generic_constraint_sites ||
      contract.terminated_generic_suffix_sites > contract.generic_suffix_sites ||
      contract.pointer_declarator_sites > contract.generic_constraint_sites ||
      contract.normalized_constraint_sites > contract.generic_constraint_sites ||
      contract.contract_violation_sites > contract.generic_constraint_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_constraint_sites !=
           contract.generic_constraint_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3LightweightGenericsConstraintLoweringReplayKey(
    const Objc3LightweightGenericsConstraintLoweringContract &contract) {
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
         kObjc3LightweightGenericsConstraintLoweringLaneContract;
}

bool IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(
    const Objc3NullabilityFlowWarningPrecisionLoweringContract &contract) {
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
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.nullability_flow_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(
    const Objc3NullabilityFlowWarningPrecisionLoweringContract &contract) {
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
         kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract;
}

bool IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(
    const Objc3ProtocolQualifiedObjectTypeLoweringContract &contract) {
  if (contract.terminated_protocol_composition_sites >
          contract.protocol_composition_sites ||
      contract.normalized_protocol_composition_sites >
          contract.protocol_qualified_object_type_sites ||
      contract.contract_violation_sites >
          contract.protocol_qualified_object_type_sites) {
    return false;
  }
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_protocol_composition_sites !=
           contract.protocol_qualified_object_type_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(
    const Objc3ProtocolQualifiedObjectTypeLoweringContract &contract) {
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
         kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract;
}

bool IsValidObjc3VarianceBridgeCastLoweringContract(
    const Objc3VarianceBridgeCastLoweringContract &contract) {
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
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.variance_bridge_cast_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3VarianceBridgeCastLoweringReplayKey(
    const Objc3VarianceBridgeCastLoweringContract &contract) {
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
         ";lane_contract=" + kObjc3VarianceBridgeCastLoweringLaneContract;
}

bool IsValidObjc3GenericMetadataAbiLoweringContract(
    const Objc3GenericMetadataAbiLoweringContract &contract) {
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
  if ((contract.contract_violation_sites > 0 ||
       contract.normalized_sites != contract.generic_metadata_abi_sites) &&
      contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3GenericMetadataAbiLoweringReplayKey(
    const Objc3GenericMetadataAbiLoweringContract &contract) {
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
         ";lane_contract=" + kObjc3GenericMetadataAbiLoweringLaneContract;
}
