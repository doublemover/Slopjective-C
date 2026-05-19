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
