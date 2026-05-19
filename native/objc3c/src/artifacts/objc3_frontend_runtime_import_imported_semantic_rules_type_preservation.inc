static void AccumulateImportedRuntimeTypeSystemPreservationRules(
    const Objc3ImportedRuntimeModuleSurface &surface,
    Objc3ImportedRuntimeMetadataSemanticRulesSummary &summary) {
  if (surface.type_system_optional_keypath_lowering_contract_present) {
    ++summary.imported_type_system_optional_keypath_module_count;
    summary.optional_send_site_count += surface.type_system_optional_send_sites;
    summary.typed_keypath_literal_site_count +=
        surface.type_system_typed_keypath_literal_sites;
    summary.live_optional_lowering_site_count +=
        surface.type_system_live_optional_lowering_sites;
    summary.live_typed_keypath_artifact_site_count +=
        surface.type_system_live_typed_keypath_artifact_sites;
    if (surface.type_system_optional_send_runtime_ready) {
      ++summary.imported_optional_runtime_ready_module_count;
    }
    if (surface.type_system_typed_keypath_descriptor_handles_ready &&
        surface.type_system_typed_keypath_runtime_execution_helper_landed) {
      ++summary.imported_typed_keypath_runtime_ready_module_count;
    }
  }
  if (surface.type_system_generic_contract_preservation_present) {
    ++summary.imported_type_system_generic_contract_module_count;
    summary.imported_generic_interface_count +=
        surface.type_system_generic_interface_count;
    summary.imported_generic_parameter_count +=
        surface.type_system_generic_parameter_count;
    summary.imported_generic_variance_annotation_count +=
        surface.type_system_generic_variance_annotation_count;
    summary.imported_generic_argument_reference_count +=
        surface.type_system_generic_argument_reference_count;
    summary.imported_protocol_qualified_generic_argument_count +=
        surface.type_system_protocol_qualified_generic_argument_count;
  }
  if (surface.type_system_nullability_contract_preservation_present) {
    ++summary.imported_type_system_nullability_contract_module_count;
    summary.imported_nullability_canonical_type_count +=
        surface.type_system_nullability_canonical_type_count;
    summary.imported_nullability_object_type_count +=
        surface.type_system_nullability_object_type_count;
    summary.imported_nullable_entry_count +=
        surface.type_system_nullable_entry_count;
    summary.imported_nonnull_entry_count +=
        surface.type_system_nonnull_entry_count;
    summary.imported_implicitly_unwrapped_entry_count +=
        surface.type_system_implicitly_unwrapped_entry_count;
    summary.imported_null_resettable_entry_count +=
        surface.type_system_null_resettable_entry_count;
    summary.imported_unspecified_nullability_entry_count +=
        surface.type_system_unspecified_nullability_entry_count;
    summary.imported_invalid_nullability_entry_count +=
        surface.type_system_invalid_nullability_entry_count;
  }
  if (surface.type_system_protocol_contract_preservation_present) {
    ++summary.imported_type_system_protocol_contract_module_count;
    summary.imported_protocol_decl_count +=
        surface.type_system_protocol_decl_count;
    summary.imported_protocol_forward_declaration_count +=
        surface.type_system_protocol_forward_declaration_count;
    summary.imported_protocol_inheritance_edge_count +=
        surface.type_system_protocol_inheritance_edge_count;
    summary.imported_protocol_required_method_count +=
        surface.type_system_protocol_required_method_count;
    summary.imported_protocol_optional_method_count +=
        surface.type_system_protocol_optional_method_count;
    summary.imported_protocol_required_property_count +=
        surface.type_system_protocol_required_property_count;
    summary.imported_protocol_optional_property_count +=
        surface.type_system_protocol_optional_property_count;
    summary.imported_class_protocol_adoption_count +=
        surface.type_system_class_protocol_adoption_count;
    summary.imported_category_protocol_adoption_count +=
        surface.type_system_category_protocol_adoption_count;
  }
}
