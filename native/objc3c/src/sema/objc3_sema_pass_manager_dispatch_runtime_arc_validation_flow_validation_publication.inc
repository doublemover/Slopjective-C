namespace {

static void FinalizeObjc3SemaDispatchRuntimeArcValidationFlowRecord(
    Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord &record) {
  record.passed_validation_count =
      Objc3SemaEvidenceCount(record.dispatch_abi_marshalling_ready) +
      Objc3SemaEvidenceCount(
          record.nil_receiver_semantics_foldability_ready) +
      Objc3SemaEvidenceCount(record.super_dispatch_method_family_ready) +
      Objc3SemaEvidenceCount(record.runtime_link_host_link_ready) +
      Objc3SemaEvidenceCount(record.retain_release_operation_ready) +
      Objc3SemaEvidenceCount(record.weak_unowned_semantics_ready) +
      Objc3SemaEvidenceCount(record.arc_diagnostics_fixit_ready) +
      Objc3SemaEvidenceCount(record.autoreleasepool_scope_ready);
  record.failed_validation_count =
      record.required_validation_count >= record.passed_validation_count
          ? (record.required_validation_count -
             record.passed_validation_count)
          : record.required_validation_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.dispatch_runtime_arc_parity_validation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.required_validation_count == 8u &&
      record.passed_validation_count == record.required_validation_count &&
      record.failed_validation_count == 0u;
}

}  // namespace
