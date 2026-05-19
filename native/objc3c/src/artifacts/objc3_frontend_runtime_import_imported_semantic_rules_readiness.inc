static void FinalizeImportedRuntimeMetadataSemanticRuleReadiness(
    Objc3ImportedRuntimeMetadataSemanticRulesSummary &summary) {
  std::sort(summary.imported_module_names_lexicographic.begin(),
            summary.imported_module_names_lexicographic.end());
  summary.runtime_owned_declaration_count =
      summary.class_record_count + summary.protocol_record_count +
      summary.category_record_count + summary.property_record_count +
      summary.method_record_count + summary.ivar_record_count;
  summary.imported_conformance_shape_landed =
      summary.source_semantic_preservation_contract_ready;
  summary.imported_dispatch_traits_landed =
      summary.source_semantic_preservation_contract_ready;
  summary.imported_effect_traits_landed =
      summary.source_semantic_preservation_contract_ready;
  summary.imported_runtime_metadata_semantics_landed =
      summary.source_semantic_preservation_contract_ready;
  const bool imported_optional_keypath_landed =
      summary.imported_type_system_optional_keypath_module_count == 0u ||
      summary.imported_type_system_optional_keypath_module_count ==
          summary.imported_optional_runtime_ready_module_count;
  const bool imported_generic_contract_landed =
      summary.imported_type_system_generic_contract_module_count == 0u ||
      summary.imported_generic_variance_annotation_count >=
          summary.imported_generic_parameter_count;
  const bool imported_nullability_contract_landed =
      summary.imported_type_system_nullability_contract_module_count == 0u ||
      summary.imported_nullability_canonical_type_count ==
          summary.imported_nullable_entry_count +
              summary.imported_nonnull_entry_count +
              summary.imported_implicitly_unwrapped_entry_count +
              summary.imported_null_resettable_entry_count +
              summary.imported_unspecified_nullability_entry_count;
  const bool imported_protocol_contract_landed =
      summary.imported_type_system_protocol_contract_module_count == 0u ||
      summary.imported_protocol_decl_count >=
          summary.imported_protocol_forward_declaration_count;
  summary.imported_type_system_type_surface_landed =
      imported_optional_keypath_landed && imported_generic_contract_landed &&
      imported_nullability_contract_landed && imported_protocol_contract_landed;
  summary.imported_optional_runtime_semantics_landed =
      summary.optional_send_site_count == 0u ||
      summary.imported_optional_runtime_ready_module_count > 0u;
  summary.imported_typed_keypath_runtime_semantics_landed =
      summary.typed_keypath_literal_site_count == 0u ||
      summary.imported_typed_keypath_runtime_ready_module_count > 0u;
  summary.ready_for_imported_metadata_semantic_rules =
      summary.source_semantic_preservation_contract_ready &&
      summary.imported_runtime_surface_inputs_loaded;
  summary.ready_for_cross_module_dispatch_equivalence =
      summary.ready_for_imported_metadata_semantic_rules;

  if (summary.ready_for_imported_metadata_semantic_rules) {
    summary.replay_key =
        BuildImportedRuntimeMetadataSemanticRulesReplayKey(summary);
  } else {
    summary.failure_reason =
        "imported runtime metadata semantic rules summary is incomplete";
  }
  if (!IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(summary) &&
      summary.failure_reason.empty()) {
    summary.failure_reason =
        "imported runtime metadata semantic rules summary is incomplete";
  }
}
