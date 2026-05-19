namespace {

static void PopulateObjc3SemaCorePublicationFlowBoundaryReadiness(
    Objc3SemaCoreSemanticParityPublicationReadinessRecord &record,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_lightweight_generic_constraint_handoff,
    bool deterministic_nullability_flow_warning_precision_handoff,
    bool deterministic_protocol_qualified_object_type_handoff,
    bool deterministic_variance_bridge_cast_handoff) {
  record.lightweight_generic_constraint_ready =
      deterministic_lightweight_generic_constraint_handoff &&
      surface.lightweight_generic_constraint_summary.generic_constraint_sites ==
          surface.lightweight_generic_constraint_sites_total &&
      surface.lightweight_generic_constraint_summary.generic_suffix_sites ==
          surface.lightweight_generic_constraint_generic_suffix_sites_total &&
      surface.lightweight_generic_constraint_summary.object_pointer_type_sites ==
          surface
              .lightweight_generic_constraint_object_pointer_type_sites_total &&
      surface.lightweight_generic_constraint_summary
              .terminated_generic_suffix_sites ==
          surface
              .lightweight_generic_constraint_terminated_generic_suffix_sites_total &&
      surface.lightweight_generic_constraint_summary.pointer_declarator_sites ==
          surface.lightweight_generic_constraint_pointer_declarator_sites_total &&
      surface.lightweight_generic_constraint_summary.normalized_constraint_sites ==
          surface.lightweight_generic_constraint_normalized_sites_total &&
      surface.lightweight_generic_constraint_summary.contract_violation_sites ==
          surface
              .lightweight_generic_constraint_contract_violation_sites_total &&
      surface.lightweight_generic_constraint_summary
              .terminated_generic_suffix_sites <=
          surface.lightweight_generic_constraint_summary
              .generic_suffix_sites &&
      surface.lightweight_generic_constraint_summary.normalized_constraint_sites <=
          surface.lightweight_generic_constraint_summary
              .generic_constraint_sites &&
      surface.lightweight_generic_constraint_summary.contract_violation_sites <=
          surface.lightweight_generic_constraint_summary
              .generic_constraint_sites &&
      surface.lightweight_generic_constraint_summary.deterministic;
  record.nullability_flow_warning_precision_ready =
      deterministic_nullability_flow_warning_precision_handoff &&
      surface.nullability_flow_warning_precision_summary.nullability_flow_sites ==
          surface.nullability_flow_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .object_pointer_type_sites ==
          surface.nullability_flow_object_pointer_type_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .nullability_suffix_sites ==
          surface.nullability_flow_nullability_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .nullable_suffix_sites ==
          surface.nullability_flow_nullable_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary.nonnull_suffix_sites ==
          surface.nullability_flow_nonnull_suffix_sites_total &&
      surface.nullability_flow_warning_precision_summary.normalized_sites ==
          surface.nullability_flow_normalized_sites_total &&
      surface.nullability_flow_warning_precision_summary
              .contract_violation_sites ==
          surface.nullability_flow_contract_violation_sites_total &&
      surface.nullability_flow_warning_precision_summary.normalized_sites <=
          surface.nullability_flow_warning_precision_summary
              .nullability_flow_sites &&
      surface.nullability_flow_warning_precision_summary.contract_violation_sites <=
          surface.nullability_flow_warning_precision_summary
              .nullability_flow_sites &&
      surface.nullability_flow_warning_precision_summary
              .nullability_suffix_sites ==
          surface.nullability_flow_warning_precision_summary
              .nullable_suffix_sites +
              surface.nullability_flow_warning_precision_summary
                  .nonnull_suffix_sites &&
      surface.nullability_flow_warning_precision_summary.deterministic;
  record.protocol_qualified_object_type_ready =
      deterministic_protocol_qualified_object_type_handoff &&
      surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites ==
          surface.protocol_qualified_object_type_sites_total &&
      surface.protocol_qualified_object_type_summary
              .protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.object_pointer_type_sites ==
          surface
              .protocol_qualified_object_type_object_pointer_type_sites_total &&
      surface.protocol_qualified_object_type_summary
              .terminated_protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_terminated_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.pointer_declarator_sites ==
          surface
              .protocol_qualified_object_type_pointer_declarator_sites_total &&
      surface.protocol_qualified_object_type_summary
              .normalized_protocol_composition_sites ==
          surface
              .protocol_qualified_object_type_normalized_protocol_composition_sites_total &&
      surface.protocol_qualified_object_type_summary.contract_violation_sites ==
          surface
              .protocol_qualified_object_type_contract_violation_sites_total &&
      surface.protocol_qualified_object_type_summary
              .terminated_protocol_composition_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_composition_sites &&
      surface.protocol_qualified_object_type_summary
              .normalized_protocol_composition_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites &&
      surface.protocol_qualified_object_type_summary.contract_violation_sites <=
          surface.protocol_qualified_object_type_summary
              .protocol_qualified_object_type_sites &&
      surface.protocol_qualified_object_type_summary.deterministic;
  record.variance_bridge_cast_ready =
      deterministic_variance_bridge_cast_handoff &&
      surface.variance_bridge_cast_summary.variance_bridge_cast_sites ==
          surface.variance_bridge_cast_sites_total &&
      surface.variance_bridge_cast_summary.protocol_composition_sites ==
          surface.variance_bridge_cast_protocol_composition_sites_total &&
      surface.variance_bridge_cast_summary.ownership_qualifier_sites ==
          surface.variance_bridge_cast_ownership_qualifier_sites_total &&
      surface.variance_bridge_cast_summary.object_pointer_type_sites ==
          surface.variance_bridge_cast_object_pointer_type_sites_total &&
      surface.variance_bridge_cast_summary.pointer_declarator_sites ==
          surface.variance_bridge_cast_pointer_declarator_sites_total &&
      surface.variance_bridge_cast_summary.normalized_sites ==
          surface.variance_bridge_cast_normalized_sites_total &&
      surface.variance_bridge_cast_summary.contract_violation_sites ==
          surface.variance_bridge_cast_contract_violation_sites_total &&
      surface.variance_bridge_cast_summary.protocol_composition_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.normalized_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.contract_violation_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.deterministic;
}

}  // namespace
