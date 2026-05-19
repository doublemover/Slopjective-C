namespace {

static void FinalizeObjc3SemaCorePublicationFlowRecord(
    Objc3SemaCoreSemanticParityPublicationReadinessRecord &record) {
  record.passed_publication_count =
      Objc3SemaEvidenceCount(record.semantic_diagnostics_ready) +
      Objc3SemaEvidenceCount(record.type_metadata_handoff_ready) +
      Objc3SemaEvidenceCount(record.interface_implementation_ready) +
      Objc3SemaEvidenceCount(record.protocol_category_composition_ready) +
      Objc3SemaEvidenceCount(record.class_protocol_category_linking_ready) +
      Objc3SemaEvidenceCount(record.selector_normalization_ready) +
      Objc3SemaEvidenceCount(record.property_attribute_ready) +
      Objc3SemaEvidenceCount(record.type_annotation_surface_ready) +
      Objc3SemaEvidenceCount(record.lightweight_generic_constraint_ready) +
      Objc3SemaEvidenceCount(
          record.nullability_flow_warning_precision_ready) +
      Objc3SemaEvidenceCount(record.protocol_qualified_object_type_ready) +
      Objc3SemaEvidenceCount(record.variance_bridge_cast_ready);
  record.failed_publication_count =
      record.required_publication_count >= record.passed_publication_count
          ? (record.required_publication_count -
             record.passed_publication_count)
          : record.required_publication_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.core_semantic_parity_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.required_publication_count == 12u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u;
}

}  // namespace
