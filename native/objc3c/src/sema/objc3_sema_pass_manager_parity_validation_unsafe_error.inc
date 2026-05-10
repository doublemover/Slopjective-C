              .concurrency_replay_race_guard_sites;
  result.unsafe_error_parity_validation_readiness_record =
      BuildObjc3SemaUnsafeErrorParityValidationReadinessRecord(
          input,
          result.parity_surface,
          result.deterministic_unsafe_pointer_extension_handoff,
          result.deterministic_inline_asm_intrinsic_governance_handoff,
          result.deterministic_ns_error_bridging_handoff,
          result.deterministic_error_diagnostics_recovery_handoff,
          result.deterministic_result_like_lowering_handoff);
  result.deterministic_unsafe_error_parity_validation_readiness_record =
      IsReadyObjc3SemaUnsafeErrorParityValidationReadinessRecord(
          result.unsafe_error_parity_validation_readiness_record);
  result.parity_surface.unsafe_error_parity_validation_readiness_record =
      result.unsafe_error_parity_validation_readiness_record;
  result.parity_surface
      .deterministic_unsafe_error_parity_validation_readiness_record =
      result.deterministic_unsafe_error_parity_validation_readiness_record;
  result.parity_surface.deterministic_unsafe_pointer_extension_handoff =
      result.parity_surface.unsafe_error_parity_validation_readiness_record
          .unsafe_pointer_extension_ready;
  result.parity_surface.deterministic_inline_asm_intrinsic_governance_handoff =
      result.parity_surface.unsafe_error_parity_validation_readiness_record
          .inline_asm_intrinsic_governance_ready;
  result.parity_surface.deterministic_ns_error_bridging_handoff =
      result.parity_surface.unsafe_error_parity_validation_readiness_record
          .ns_error_bridging_ready;
  result.parity_surface.deterministic_error_diagnostics_recovery_handoff =
      result.parity_surface.unsafe_error_parity_validation_readiness_record
          .error_diagnostics_recovery_ready;
  result.parity_surface.deterministic_result_like_lowering_handoff =
      result.parity_surface.unsafe_error_parity_validation_readiness_record
          .result_like_lowering_ready;
