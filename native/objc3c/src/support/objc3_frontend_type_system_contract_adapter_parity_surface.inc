Objc3FrontendTypeSystemParitySurfaceRecord
BuildFrontendTypeSystemParitySurfaceRecord(
    const Objc3SemaParityContractSurface &surface) {
  Objc3FrontendTypeSystemParitySurfaceRecord record;
  record.lightweight_generic_constraint_sites_total =
      surface.lightweight_generic_constraint_sites_total;
  record.lightweight_generic_constraint_generic_suffix_sites_total =
      surface.lightweight_generic_constraint_generic_suffix_sites_total;
  record.lightweight_generic_constraint_object_pointer_type_sites_total =
      surface.lightweight_generic_constraint_object_pointer_type_sites_total;
  record.lightweight_generic_constraint_terminated_generic_suffix_sites_total =
      surface.lightweight_generic_constraint_terminated_generic_suffix_sites_total;
  record.lightweight_generic_constraint_pointer_declarator_sites_total =
      surface.lightweight_generic_constraint_pointer_declarator_sites_total;
  record.lightweight_generic_constraint_normalized_sites_total =
      surface.lightweight_generic_constraint_normalized_sites_total;
  record.lightweight_generic_constraint_contract_violation_sites_total =
      surface.lightweight_generic_constraint_contract_violation_sites_total;
  record.lightweight_generic_constraint_deterministic =
      surface.lightweight_generic_constraint_summary.deterministic;
  record.deterministic_lightweight_generic_constraint_handoff =
      surface.deterministic_lightweight_generic_constraint_handoff;
  record.nullability_flow_sites_total =
      surface.nullability_flow_sites_total;
  record.nullability_flow_object_pointer_type_sites_total =
      surface.nullability_flow_object_pointer_type_sites_total;
  record.nullability_flow_nullability_suffix_sites_total =
      surface.nullability_flow_nullability_suffix_sites_total;
  record.nullability_flow_nullable_suffix_sites_total =
      surface.nullability_flow_nullable_suffix_sites_total;
  record.nullability_flow_nonnull_suffix_sites_total =
      surface.nullability_flow_nonnull_suffix_sites_total;
  record.nullability_flow_normalized_sites_total =
      surface.nullability_flow_normalized_sites_total;
  record.nullability_flow_contract_violation_sites_total =
      surface.nullability_flow_contract_violation_sites_total;
  record.nullability_flow_warning_precision_deterministic =
      surface.nullability_flow_warning_precision_summary.deterministic;
  record.deterministic_nullability_flow_warning_precision_handoff =
      surface.deterministic_nullability_flow_warning_precision_handoff;
  record.protocol_qualified_object_type_sites_total =
      surface.protocol_qualified_object_type_sites_total;
  record.protocol_qualified_object_type_protocol_composition_sites_total =
      surface.protocol_qualified_object_type_protocol_composition_sites_total;
  record.protocol_qualified_object_type_object_pointer_type_sites_total =
      surface.protocol_qualified_object_type_object_pointer_type_sites_total;
  record.protocol_qualified_object_type_terminated_protocol_composition_sites_total =
      surface.protocol_qualified_object_type_terminated_protocol_composition_sites_total;
  record.protocol_qualified_object_type_pointer_declarator_sites_total =
      surface.protocol_qualified_object_type_pointer_declarator_sites_total;
  record.protocol_qualified_object_type_normalized_protocol_composition_sites_total =
      surface.protocol_qualified_object_type_normalized_protocol_composition_sites_total;
  record.protocol_qualified_object_type_contract_violation_sites_total =
      surface.protocol_qualified_object_type_contract_violation_sites_total;
  record.protocol_qualified_object_type_deterministic =
      surface.protocol_qualified_object_type_summary.deterministic;
  record.deterministic_protocol_qualified_object_type_handoff =
      surface.deterministic_protocol_qualified_object_type_handoff;
  record.variance_bridge_cast_sites_total =
      surface.variance_bridge_cast_sites_total;
  record.variance_bridge_cast_protocol_composition_sites_total =
      surface.variance_bridge_cast_protocol_composition_sites_total;
  record.variance_bridge_cast_ownership_qualifier_sites_total =
      surface.variance_bridge_cast_ownership_qualifier_sites_total;
  record.variance_bridge_cast_object_pointer_type_sites_total =
      surface.variance_bridge_cast_object_pointer_type_sites_total;
  record.variance_bridge_cast_pointer_declarator_sites_total =
      surface.variance_bridge_cast_pointer_declarator_sites_total;
  record.variance_bridge_cast_normalized_sites_total =
      surface.variance_bridge_cast_normalized_sites_total;
  record.variance_bridge_cast_contract_violation_sites_total =
      surface.variance_bridge_cast_contract_violation_sites_total;
  record.variance_bridge_cast_deterministic =
      surface.variance_bridge_cast_summary.deterministic;
  record.deterministic_variance_bridge_cast_handoff =
      surface.deterministic_variance_bridge_cast_handoff;
  record.generic_metadata_abi_sites_total =
      surface.generic_metadata_abi_sites_total;
  record.generic_metadata_abi_generic_suffix_sites_total =
      surface.generic_metadata_abi_generic_suffix_sites_total;
  record.generic_metadata_abi_protocol_composition_sites_total =
      surface.generic_metadata_abi_protocol_composition_sites_total;
  record.generic_metadata_abi_ownership_qualifier_sites_total =
      surface.generic_metadata_abi_ownership_qualifier_sites_total;
  record.generic_metadata_abi_object_pointer_type_sites_total =
      surface.generic_metadata_abi_object_pointer_type_sites_total;
  record.generic_metadata_abi_pointer_declarator_sites_total =
      surface.generic_metadata_abi_pointer_declarator_sites_total;
  record.generic_metadata_abi_normalized_sites_total =
      surface.generic_metadata_abi_normalized_sites_total;
  record.generic_metadata_abi_contract_violation_sites_total =
      surface.generic_metadata_abi_contract_violation_sites_total;
  record.generic_metadata_abi_deterministic =
      surface.generic_metadata_abi_summary.deterministic;
  record.deterministic_generic_metadata_abi_handoff =
      surface.deterministic_generic_metadata_abi_handoff;
  return record;
}

Objc3FrontendLightweightGenericsConstraintLoweringContractRecord
BuildLightweightGenericsConstraintLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  return BuildLightweightGenericsConstraintLoweringContract(
      BuildFrontendTypeSystemParitySurfaceRecord(sema_parity_surface));
}

Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord
BuildNullabilityFlowWarningPrecisionLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  return BuildNullabilityFlowWarningPrecisionLoweringContract(
      BuildFrontendTypeSystemParitySurfaceRecord(sema_parity_surface));
}

Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord
BuildProtocolQualifiedObjectTypeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  return BuildProtocolQualifiedObjectTypeLoweringContract(
      BuildFrontendTypeSystemParitySurfaceRecord(sema_parity_surface));
}

Objc3FrontendVarianceBridgeCastLoweringContractRecord
BuildVarianceBridgeCastLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  return BuildVarianceBridgeCastLoweringContract(
      BuildFrontendTypeSystemParitySurfaceRecord(sema_parity_surface));
}

Objc3FrontendGenericMetadataAbiLoweringContractRecord
BuildGenericMetadataAbiLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  return BuildGenericMetadataAbiLoweringContract(
      BuildFrontendTypeSystemParitySurfaceRecord(sema_parity_surface));
}
