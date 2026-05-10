  record.passed_validation_count =
      Objc3SemaEvidenceCount(record.unwind_cleanup_ready) +
      Objc3SemaEvidenceCount(record.async_continuation_ready) +
      Objc3SemaEvidenceCount(record.symbol_graph_scope_resolution_ready) +
      Objc3SemaEvidenceCount(record.method_lookup_override_conflict_ready) +
      Objc3SemaEvidenceCount(record.property_synthesis_ivar_binding_ready) +
      Objc3SemaEvidenceCount(
          record.id_class_sel_object_pointer_type_checking_ready);
  record.failed_validation_count =
      record.required_validation_count >= record.passed_validation_count
          ? (record.required_validation_count -
             record.passed_validation_count)
          : record.required_validation_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.control_binding_parity_validation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.required_validation_count == 6u &&
      record.passed_validation_count == record.required_validation_count &&
      record.failed_validation_count == 0u;
