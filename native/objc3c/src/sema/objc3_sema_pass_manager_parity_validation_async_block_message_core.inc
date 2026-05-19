  result.async_block_message_parity_validation_readiness_record =
      BuildObjc3SemaAsyncBlockMessageParityValidationReadinessRecord(
          input,
          result.parity_surface,
          result.deterministic_await_lowering_suspension_state_lowering_handoff,
          result.deterministic_block_literal_capture_semantics_handoff,
          result.deterministic_block_abi_invoke_trampoline_handoff,
          result.deterministic_block_storage_escape_handoff,
          result.deterministic_block_copy_dispose_handoff,
          result.deterministic_block_determinism_perf_baseline_handoff,
          result.deterministic_message_send_selector_lowering_handoff);
  result.deterministic_async_block_message_parity_validation_readiness_record =
      IsReadyObjc3SemaAsyncBlockMessageParityValidationReadinessRecord(
          result.async_block_message_parity_validation_readiness_record);
  result.parity_surface.async_block_message_parity_validation_readiness_record =
      result.async_block_message_parity_validation_readiness_record;
  result.parity_surface
      .deterministic_async_block_message_parity_validation_readiness_record =
      result.deterministic_async_block_message_parity_validation_readiness_record;
  result.parity_surface
      .deterministic_await_lowering_suspension_state_lowering_handoff =
      result.parity_surface
          .async_block_message_parity_validation_readiness_record
          .await_lowering_suspension_state_lowering_ready;
