  record.passed_validation_count =
      Objc3SemaEvidenceCount(
          record.await_lowering_suspension_state_lowering_ready) +
      Objc3SemaEvidenceCount(record.block_literal_capture_semantics_ready) +
      Objc3SemaEvidenceCount(record.block_abi_invoke_trampoline_ready) +
      Objc3SemaEvidenceCount(record.block_storage_escape_ready) +
      Objc3SemaEvidenceCount(record.block_copy_dispose_ready) +
      Objc3SemaEvidenceCount(record.block_determinism_perf_baseline_ready) +
      Objc3SemaEvidenceCount(record.message_send_selector_lowering_ready);
  record.failed_validation_count =
      record.required_validation_count >= record.passed_validation_count
          ? (record.required_validation_count -
             record.passed_validation_count)
          : record.required_validation_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.async_block_message_parity_validation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.required_validation_count == 7u &&
      record.passed_validation_count == record.required_validation_count &&
      record.failed_validation_count == 0u;
