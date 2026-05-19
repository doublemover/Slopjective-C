std::string BuildImportedRuntimeMetadataSemanticRulesSummaryJson(
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary &summary) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("source_semantic_preservation_contract_id",
                     summary.source_semantic_preservation_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("input_model", summary.input_model);
  object.StringArrayField("imported_module_names_lexicographic",
                          summary.imported_module_names_lexicographic);
  object.SizeField("imported_input_path_count",
                   summary.imported_input_path_count);
  object.SizeField("imported_module_count", summary.imported_module_count);
  object.SizeField("class_record_count", summary.class_record_count);
  object.SizeField("protocol_record_count", summary.protocol_record_count);
  object.SizeField("category_record_count", summary.category_record_count);
  object.SizeField("property_record_count", summary.property_record_count);
  object.SizeField("method_record_count", summary.method_record_count);
  object.SizeField("ivar_record_count", summary.ivar_record_count);
  object.SizeField("runtime_owned_declaration_count",
                   summary.runtime_owned_declaration_count);
  object.SizeField("superclass_edge_count", summary.superclass_edge_count);
  object.SizeField("protocol_conformance_edge_count",
                   summary.protocol_conformance_edge_count);
  object.SizeField("category_attachment_count",
                   summary.category_attachment_count);
  object.SizeField("property_accessor_trait_count",
                   summary.property_accessor_trait_count);
  object.SizeField("property_ivar_binding_trait_count",
                   summary.property_ivar_binding_trait_count);
  object.SizeField("method_selector_trait_count",
                   summary.method_selector_trait_count);
  object.SizeField("class_method_trait_count", summary.class_method_trait_count);
  object.SizeField("instance_method_trait_count",
                   summary.instance_method_trait_count);
  object.SizeField("implemented_method_count",
                   summary.implemented_method_count);
  object.SizeField("declaration_only_method_count",
                   summary.declaration_only_method_count);
  object.SizeField("property_attribute_profile_count",
                   summary.property_attribute_profile_count);
  object.SizeField("ownership_effect_profile_count",
                   summary.ownership_effect_profile_count);
  object.SizeField("executable_binding_trait_count",
                   summary.executable_binding_trait_count);
  object.SizeField("optional_send_site_count", summary.optional_send_site_count);
  object.SizeField("typed_keypath_literal_site_count",
                   summary.typed_keypath_literal_site_count);
  object.SizeField("live_optional_lowering_site_count",
                   summary.live_optional_lowering_site_count);
  object.SizeField("live_typed_keypath_artifact_site_count",
                   summary.live_typed_keypath_artifact_site_count);
  object.SizeField("imported_type_system_optional_keypath_module_count",
                   summary.imported_type_system_optional_keypath_module_count);
  object.SizeField("imported_optional_runtime_ready_module_count",
                   summary.imported_optional_runtime_ready_module_count);
  object.SizeField("imported_typed_keypath_runtime_ready_module_count",
                   summary.imported_typed_keypath_runtime_ready_module_count);
  object.SizeField("imported_type_system_generic_contract_module_count",
                   summary.imported_type_system_generic_contract_module_count);
  object.SizeField("imported_generic_interface_count",
                   summary.imported_generic_interface_count);
  object.SizeField("imported_generic_parameter_count",
                   summary.imported_generic_parameter_count);
  object.SizeField("imported_generic_variance_annotation_count",
                   summary.imported_generic_variance_annotation_count);
  object.SizeField("imported_generic_argument_reference_count",
                   summary.imported_generic_argument_reference_count);
  object.SizeField("imported_protocol_qualified_generic_argument_count",
                   summary.imported_protocol_qualified_generic_argument_count);
  object.SizeField("imported_type_system_nullability_contract_module_count",
                   summary.imported_type_system_nullability_contract_module_count);
  object.SizeField("imported_nullability_canonical_type_count",
                   summary.imported_nullability_canonical_type_count);
  object.SizeField("imported_nullability_object_type_count",
                   summary.imported_nullability_object_type_count);
  object.SizeField("imported_nullable_entry_count",
                   summary.imported_nullable_entry_count);
  object.SizeField("imported_nonnull_entry_count",
                   summary.imported_nonnull_entry_count);
  object.SizeField("imported_implicitly_unwrapped_entry_count",
                   summary.imported_implicitly_unwrapped_entry_count);
  object.SizeField("imported_null_resettable_entry_count",
                   summary.imported_null_resettable_entry_count);
  object.SizeField("imported_unspecified_nullability_entry_count",
                   summary.imported_unspecified_nullability_entry_count);
  object.SizeField("imported_invalid_nullability_entry_count",
                   summary.imported_invalid_nullability_entry_count);
  object.SizeField("imported_type_system_protocol_contract_module_count",
                   summary.imported_type_system_protocol_contract_module_count);
  object.SizeField("imported_protocol_decl_count",
                   summary.imported_protocol_decl_count);
  object.SizeField("imported_protocol_forward_declaration_count",
                   summary.imported_protocol_forward_declaration_count);
  object.SizeField("imported_protocol_inheritance_edge_count",
                   summary.imported_protocol_inheritance_edge_count);
  object.SizeField("imported_protocol_required_method_count",
                   summary.imported_protocol_required_method_count);
  object.SizeField("imported_protocol_optional_method_count",
                   summary.imported_protocol_optional_method_count);
  object.SizeField("imported_protocol_required_property_count",
                   summary.imported_protocol_required_property_count);
  object.SizeField("imported_protocol_optional_property_count",
                   summary.imported_protocol_optional_property_count);
  object.SizeField("imported_class_protocol_adoption_count",
                   summary.imported_class_protocol_adoption_count);
  object.SizeField("imported_category_protocol_adoption_count",
                   summary.imported_category_protocol_adoption_count);
  object.BoolField(
      "ready",
      IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(summary));
  object.BoolField("fail_closed", summary.fail_closed);
  object.BoolField("source_semantic_preservation_contract_ready",
                   summary.source_semantic_preservation_contract_ready);
  object.BoolField("semantic_surface_published",
                   summary.semantic_surface_published);
  object.BoolField("imported_runtime_surface_inputs_present",
                   summary.imported_runtime_surface_inputs_present);
  object.BoolField("imported_runtime_surface_inputs_loaded",
                   summary.imported_runtime_surface_inputs_loaded);
  object.BoolField("imported_conformance_shape_landed",
                   summary.imported_conformance_shape_landed);
  object.BoolField("imported_dispatch_traits_landed",
                   summary.imported_dispatch_traits_landed);
  object.BoolField("imported_effect_traits_landed",
                   summary.imported_effect_traits_landed);
  object.BoolField("imported_runtime_metadata_semantics_landed",
                   summary.imported_runtime_metadata_semantics_landed);
  object.BoolField("imported_type_system_type_surface_landed",
                   summary.imported_type_system_type_surface_landed);
  object.BoolField("imported_optional_runtime_semantics_landed",
                   summary.imported_optional_runtime_semantics_landed);
  object.BoolField("imported_typed_keypath_runtime_semantics_landed",
                   summary.imported_typed_keypath_runtime_semantics_landed);
  object.BoolField("ready_for_imported_metadata_semantic_rules",
                   summary.ready_for_imported_metadata_semantic_rules);
  object.BoolField("ready_for_cross_module_dispatch_equivalence",
                   summary.ready_for_cross_module_dispatch_equivalence);
  object.StringField("replay_key", summary.replay_key);
  object.StringField("failure_reason", summary.failure_reason);
  object.End();
  return out.str();
}
