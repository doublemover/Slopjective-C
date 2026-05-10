  result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);
  result.deterministic_type_metadata_handoff =
      IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);
  result.deterministic_interface_implementation_handoff =
      result.type_metadata_handoff.interface_implementation_summary.deterministic &&
      result.integration_surface.interface_implementation_summary.deterministic &&
      result.integration_surface.interface_implementation_summary.declared_interfaces ==
          result.type_metadata_handoff.interface_implementation_summary.declared_interfaces &&
      result.integration_surface.interface_implementation_summary.declared_implementations ==
          result.type_metadata_handoff.interface_implementation_summary.declared_implementations &&
      result.integration_surface.interface_implementation_summary.resolved_interfaces ==
          result.type_metadata_handoff.interface_implementation_summary.resolved_interfaces &&
      result.integration_surface.interface_implementation_summary.resolved_implementations ==
          result.type_metadata_handoff.interface_implementation_summary.resolved_implementations &&
      result.integration_surface.interface_implementation_summary.interface_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.interface_method_symbols &&
      result.integration_surface.interface_implementation_summary.implementation_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.implementation_method_symbols &&
      result.integration_surface.interface_implementation_summary.linked_implementation_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.linked_implementation_symbols;
  result.deterministic_protocol_category_composition_handoff =
      result.type_metadata_handoff.protocol_category_composition_summary.deterministic &&
      result.integration_surface.protocol_category_composition_summary.deterministic &&
      result.integration_surface.protocol_category_composition_summary.protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_sites &&
      result.integration_surface.protocol_category_composition_summary.protocol_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_symbols &&
      result.integration_surface.protocol_category_composition_summary.category_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_sites &&
      result.integration_surface.protocol_category_composition_summary.category_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_symbols &&
      result.integration_surface.protocol_category_composition_summary.invalid_protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.invalid_protocol_composition_sites;
  result.class_protocol_category_linking_summary = result.integration_surface.class_protocol_category_linking_summary;
  result.deterministic_class_protocol_category_linking_handoff =
      result.type_metadata_handoff.class_protocol_category_linking_summary.deterministic &&
      result.integration_surface.class_protocol_category_linking_summary.deterministic &&
      IsEquivalentClassProtocolCategoryLinkingSummary(
          result.integration_surface.class_protocol_category_linking_summary,
          result.type_metadata_handoff.class_protocol_category_linking_summary) &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.resolved_interfaces <=
          result.type_metadata_handoff.class_protocol_category_linking_summary.declared_interfaces &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.resolved_implementations <=
          result.type_metadata_handoff.class_protocol_category_linking_summary.declared_implementations &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.interface_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.interface_method_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.implementation_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.implementation_method_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.linked_implementation_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.linked_implementation_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_sites &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.protocol_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.category_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_sites &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.category_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.invalid_protocol_composition_sites &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.invalid_protocol_composition_sites <=
          result.type_metadata_handoff.class_protocol_category_linking_summary.total_composition_sites();
  result.selector_normalization_summary = result.integration_surface.selector_normalization_summary;
  result.deterministic_selector_normalization_handoff =
      result.type_metadata_handoff.selector_normalization_summary.deterministic &&
      result.integration_surface.selector_normalization_summary.deterministic &&
      IsEquivalentSelectorNormalizationSummary(result.integration_surface.selector_normalization_summary,
                                               result.type_metadata_handoff.selector_normalization_summary) &&
      result.type_metadata_handoff.selector_normalization_summary.normalized_methods <=
          result.type_metadata_handoff.selector_normalization_summary.methods_total &&
      result.type_metadata_handoff.selector_normalization_summary.selector_parameter_piece_entries <=
          result.type_metadata_handoff.selector_normalization_summary.selector_piece_entries &&
      result.type_metadata_handoff.selector_normalization_summary.contract_violations() <=
          result.type_metadata_handoff.selector_normalization_summary.methods_total;
  result.property_attribute_summary = result.integration_surface.property_attribute_summary;
  result.deterministic_property_attribute_handoff =
      result.type_metadata_handoff.property_attribute_summary.deterministic &&
      result.integration_surface.property_attribute_summary.deterministic &&
      IsEquivalentPropertyAttributeSummary(result.integration_surface.property_attribute_summary,
                                           result.type_metadata_handoff.property_attribute_summary) &&
      result.type_metadata_handoff.property_attribute_summary.getter_modifiers <=
          result.type_metadata_handoff.property_attribute_summary.properties_total &&
      result.type_metadata_handoff.property_attribute_summary.setter_modifiers <=
          result.type_metadata_handoff.property_attribute_summary.properties_total;
  result.type_annotation_surface_summary = result.integration_surface.type_annotation_surface_summary;
  result.deterministic_type_annotation_surface_handoff =
      result.type_metadata_handoff.type_annotation_surface_summary.deterministic &&
      result.integration_surface.type_annotation_surface_summary.deterministic &&
      IsEquivalentTypeAnnotationSurfaceSummary(result.integration_surface.type_annotation_surface_summary,
                                               result.type_metadata_handoff.type_annotation_surface_summary) &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_generic_suffix_sites <=
          result.type_metadata_handoff.type_annotation_surface_summary.generic_suffix_sites &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites <=
          result.type_metadata_handoff.type_annotation_surface_summary.pointer_declarator_sites &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites <=
          result.type_metadata_handoff.type_annotation_surface_summary.nullability_suffix_sites &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites <=
          result.type_metadata_handoff.type_annotation_surface_summary.ownership_qualifier_sites &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_type_annotation_sites() <=
          result.type_metadata_handoff.type_annotation_surface_summary.total_type_annotation_sites();
  result.lightweight_generic_constraint_summary =
      result.integration_surface.lightweight_generic_constraint_summary;
  result.deterministic_lightweight_generic_constraint_handoff =
      result.type_metadata_handoff.lightweight_generic_constraint_summary.deterministic &&
      result.integration_surface.lightweight_generic_constraint_summary.deterministic &&
      IsEquivalentLightweightGenericConstraintSummary(
          result.integration_surface.lightweight_generic_constraint_summary,
          result.type_metadata_handoff.lightweight_generic_constraint_summary) &&
      result.type_metadata_handoff.lightweight_generic_constraint_summary.terminated_generic_suffix_sites <=
          result.type_metadata_handoff.lightweight_generic_constraint_summary.generic_suffix_sites &&
      result.type_metadata_handoff.lightweight_generic_constraint_summary.normalized_constraint_sites <=
          result.type_metadata_handoff.lightweight_generic_constraint_summary.generic_constraint_sites &&
      result.type_metadata_handoff.lightweight_generic_constraint_summary.contract_violation_sites <=
          result.type_metadata_handoff.lightweight_generic_constraint_summary.generic_constraint_sites;
  result.nullability_flow_warning_precision_summary =
      result.integration_surface.nullability_flow_warning_precision_summary;
  result.deterministic_nullability_flow_warning_precision_handoff =
      result.type_metadata_handoff.nullability_flow_warning_precision_summary.deterministic &&
      result.integration_surface.nullability_flow_warning_precision_summary.deterministic &&
      IsEquivalentNullabilityFlowWarningPrecisionSummary(
          result.integration_surface.nullability_flow_warning_precision_summary,
          result.type_metadata_handoff.nullability_flow_warning_precision_summary) &&
      result.type_metadata_handoff.nullability_flow_warning_precision_summary.normalized_sites <=
          result.type_metadata_handoff.nullability_flow_warning_precision_summary.nullability_flow_sites &&
      result.type_metadata_handoff.nullability_flow_warning_precision_summary.contract_violation_sites <=
          result.type_metadata_handoff.nullability_flow_warning_precision_summary.nullability_flow_sites &&
      result.type_metadata_handoff.nullability_flow_warning_precision_summary.nullability_suffix_sites ==
          result.type_metadata_handoff.nullability_flow_warning_precision_summary.nullable_suffix_sites +
              result.type_metadata_handoff.nullability_flow_warning_precision_summary.nonnull_suffix_sites;
  result.protocol_qualified_object_type_summary =
      result.integration_surface.protocol_qualified_object_type_summary;
  result.deterministic_protocol_qualified_object_type_handoff =
      result.type_metadata_handoff.protocol_qualified_object_type_summary.deterministic &&
      result.integration_surface.protocol_qualified_object_type_summary.deterministic &&
      IsEquivalentProtocolQualifiedObjectTypeSummary(
          result.integration_surface.protocol_qualified_object_type_summary,
          result.type_metadata_handoff.protocol_qualified_object_type_summary) &&
      result.type_metadata_handoff.protocol_qualified_object_type_summary.terminated_protocol_composition_sites <=
          result.type_metadata_handoff.protocol_qualified_object_type_summary.protocol_composition_sites &&
      result.type_metadata_handoff.protocol_qualified_object_type_summary.normalized_protocol_composition_sites <=
          result.type_metadata_handoff.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites &&
      result.type_metadata_handoff.protocol_qualified_object_type_summary.contract_violation_sites <=
          result.type_metadata_handoff.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites;
  result.variance_bridge_cast_summary = result.integration_surface.variance_bridge_cast_summary;
  result.deterministic_variance_bridge_cast_handoff =
      result.type_metadata_handoff.variance_bridge_cast_summary.deterministic &&
      result.integration_surface.variance_bridge_cast_summary.deterministic &&
      IsEquivalentVarianceBridgeCastSummary(result.integration_surface.variance_bridge_cast_summary,
                                            result.type_metadata_handoff.variance_bridge_cast_summary) &&
      result.type_metadata_handoff.variance_bridge_cast_summary.protocol_composition_sites <=
          result.type_metadata_handoff.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      result.type_metadata_handoff.variance_bridge_cast_summary.normalized_sites <=
          result.type_metadata_handoff.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      result.type_metadata_handoff.variance_bridge_cast_summary.contract_violation_sites <=
          result.type_metadata_handoff.variance_bridge_cast_summary.variance_bridge_cast_sites;
  result.generic_metadata_abi_summary = result.integration_surface.generic_metadata_abi_summary;
  result.deterministic_generic_metadata_abi_handoff =
      result.type_metadata_handoff.generic_metadata_abi_summary.deterministic &&
      result.integration_surface.generic_metadata_abi_summary.deterministic &&
      IsEquivalentGenericMetadataAbiSummary(result.integration_surface.generic_metadata_abi_summary,
                                            result.type_metadata_handoff.generic_metadata_abi_summary) &&
      result.type_metadata_handoff.generic_metadata_abi_summary.generic_suffix_sites <=
          result.type_metadata_handoff.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      result.type_metadata_handoff.generic_metadata_abi_summary.protocol_composition_sites <=
          result.type_metadata_handoff.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      result.type_metadata_handoff.generic_metadata_abi_summary.normalized_sites <=
          result.type_metadata_handoff.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      result.type_metadata_handoff.generic_metadata_abi_summary.contract_violation_sites <=
          result.type_metadata_handoff.generic_metadata_abi_summary.generic_metadata_abi_sites;
