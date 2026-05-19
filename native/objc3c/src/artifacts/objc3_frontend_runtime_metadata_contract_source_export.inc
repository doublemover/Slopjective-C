void ApplyRuntimeMetadataSourceOwnershipContract(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership) {
  ir_frontend_metadata.runtime_metadata_source_ownership_contract_id =
      runtime_metadata_source_ownership.contract_id;
  ir_frontend_metadata.runtime_metadata_source_schema =
      runtime_metadata_source_ownership.canonical_source_schema;
  ir_frontend_metadata.runtime_metadata_ivar_source_model =
      runtime_metadata_source_ownership.ivar_record_source_model;
  ir_frontend_metadata.runtime_metadata_class_record_count =
      runtime_metadata_source_ownership.class_record_count;
  ir_frontend_metadata.runtime_metadata_protocol_record_count =
      runtime_metadata_source_ownership.protocol_record_count;
  ir_frontend_metadata.runtime_metadata_category_interface_record_count =
      runtime_metadata_source_ownership.category_interface_record_count;
  ir_frontend_metadata.runtime_metadata_category_implementation_record_count =
      runtime_metadata_source_ownership.category_implementation_record_count;
  ir_frontend_metadata.runtime_metadata_property_record_count =
      runtime_metadata_source_ownership.property_record_count;
  ir_frontend_metadata.runtime_metadata_method_record_count =
      runtime_metadata_source_ownership.method_record_count;
  ir_frontend_metadata.runtime_metadata_ivar_record_count =
      runtime_metadata_source_ownership.ivar_record_count;
  ir_frontend_metadata.frontend_owns_runtime_metadata_source_records =
      runtime_metadata_source_ownership
          .frontend_owns_runtime_metadata_source_records;
  ir_frontend_metadata.runtime_metadata_source_records_ready_for_lowering =
      runtime_metadata_source_ownership
          .runtime_metadata_source_records_ready_for_lowering;
  ir_frontend_metadata.native_runtime_library_present =
      runtime_metadata_source_ownership.native_runtime_library_present;
  ir_frontend_metadata.runtime_metadata_source_boundary_fail_closed =
      runtime_metadata_source_ownership.fail_closed;
  ir_frontend_metadata.runtime_link_test_only =
      runtime_metadata_source_ownership.runtime_link_test_only;
  ir_frontend_metadata.deterministic_runtime_metadata_source_schema =
      runtime_metadata_source_ownership.deterministic_source_schema;
}

void ApplyRuntimeExportLegalityContract(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality) {
  ir_frontend_metadata.runtime_export_legality_contract_id =
      runtime_export_legality.contract_id;
  ir_frontend_metadata.runtime_export_semantic_boundary_frozen =
      runtime_export_legality.semantic_boundary_frozen;
  ir_frontend_metadata.runtime_export_metadata_export_enforcement_ready =
      runtime_export_legality.metadata_export_enforcement_ready;
  ir_frontend_metadata.runtime_export_fail_closed =
      runtime_export_legality.fail_closed;
  ir_frontend_metadata
      .runtime_export_duplicate_runtime_identity_enforcement_pending =
      runtime_export_legality.duplicate_runtime_identity_enforcement_pending;
  ir_frontend_metadata
      .runtime_export_incomplete_declaration_export_blocking_pending =
      runtime_export_legality.incomplete_declaration_export_blocking_pending;
  ir_frontend_metadata
      .runtime_export_illegal_redeclaration_mix_export_blocking_pending =
      runtime_export_legality
          .illegal_redeclaration_mix_export_blocking_pending;
  ir_frontend_metadata.runtime_export_class_record_count =
      runtime_export_legality.class_record_count;
  ir_frontend_metadata.runtime_export_protocol_record_count =
      runtime_export_legality.protocol_record_count;
  ir_frontend_metadata.runtime_export_category_record_count =
      runtime_export_legality.category_record_count;
  ir_frontend_metadata.runtime_export_property_record_count =
      runtime_export_legality.property_record_count;
  ir_frontend_metadata.runtime_export_method_record_count =
      runtime_export_legality.method_record_count;
  ir_frontend_metadata.runtime_export_ivar_record_count =
      runtime_export_legality.ivar_record_count;
  ir_frontend_metadata.runtime_export_invalid_protocol_composition_sites =
      runtime_export_legality.invalid_protocol_composition_sites;
  ir_frontend_metadata.runtime_export_property_attribute_invalid_entries =
      runtime_export_legality.property_attribute_invalid_entries;
  ir_frontend_metadata.runtime_export_property_attribute_contract_violations =
      runtime_export_legality.property_attribute_contract_violations;
  ir_frontend_metadata.runtime_export_invalid_type_annotation_sites =
      runtime_export_legality.invalid_type_annotation_sites;
  ir_frontend_metadata.runtime_export_property_ivar_binding_missing =
      runtime_export_legality.property_ivar_binding_missing;
  ir_frontend_metadata.runtime_export_property_ivar_binding_conflicts =
      runtime_export_legality.property_ivar_binding_conflicts;
  ir_frontend_metadata.runtime_export_implementation_resolution_misses =
      runtime_export_legality.implementation_resolution_misses;
  ir_frontend_metadata.runtime_export_method_resolution_misses =
      runtime_export_legality.method_resolution_misses;
  ir_frontend_metadata.runtime_export_boundary_ready =
      IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality);
}

void ApplyRuntimeExportEnforcementContract(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  ir_frontend_metadata.runtime_export_enforcement_contract_id =
      runtime_export_enforcement.contract_id;
  ir_frontend_metadata.runtime_export_metadata_completeness_enforced =
      runtime_export_enforcement.metadata_completeness_enforced;
  ir_frontend_metadata
      .runtime_export_duplicate_runtime_identity_suppression_enforced =
      runtime_export_enforcement
          .duplicate_runtime_identity_suppression_enforced;
  ir_frontend_metadata
      .runtime_export_illegal_redeclaration_mix_blocking_enforced =
      runtime_export_enforcement
          .illegal_redeclaration_mix_blocking_enforced;
  ir_frontend_metadata.runtime_export_metadata_shape_drift_blocking_enforced =
      runtime_export_enforcement.metadata_shape_drift_blocking_enforced;
  ir_frontend_metadata.runtime_export_enforcement_fail_closed =
      runtime_export_enforcement.fail_closed;
  ir_frontend_metadata.runtime_export_ready_for_runtime_export =
      runtime_export_enforcement.ready_for_runtime_export;
  ir_frontend_metadata.runtime_export_duplicate_runtime_identity_sites =
      runtime_export_enforcement.duplicate_runtime_identity_sites;
  ir_frontend_metadata.runtime_export_incomplete_declaration_sites =
      runtime_export_enforcement.incomplete_declaration_sites;
  ir_frontend_metadata.runtime_export_illegal_redeclaration_mix_sites =
      runtime_export_enforcement.illegal_redeclaration_mix_sites;
  ir_frontend_metadata.runtime_export_metadata_shape_drift_sites =
      runtime_export_enforcement.metadata_shape_drift_sites;
}
