              .cancellation_check_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_handler_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .suspension_point_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_propagation_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_check_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_propagation_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_handler_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .normalized_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_propagation_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .normalized_sites +
              result.type_metadata_handoff.task_runtime_cancellation_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites;
  result.concurrency_replay_race_guard_summary =
      result.integration_surface.concurrency_replay_race_guard_summary;
  result.deterministic_concurrency_replay_race_guard_handoff =
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .deterministic &&
      result.integration_surface.concurrency_replay_race_guard_summary
          .deterministic &&
      IsEquivalentConcurrencyReplayRaceGuardSummary(
          result.integration_surface.concurrency_replay_race_guard_summary,
          result.type_metadata_handoff
              .concurrency_replay_race_guard_summary) &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .replay_proof_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .race_guard_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .task_handoff_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .actor_isolation_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .guard_blocked_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites +
              result.type_metadata_handoff
                  .concurrency_replay_race_guard_summary
                  .guard_blocked_sites ==
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_sites;
  result.unsafe_pointer_extension_summary =
      result.integration_surface.unsafe_pointer_extension_summary;
  result.deterministic_unsafe_pointer_extension_handoff =
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .deterministic &&
      result.integration_surface.unsafe_pointer_extension_summary
          .deterministic &&
      IsEquivalentUnsafePointerExtensionSummary(
          result.integration_surface.unsafe_pointer_extension_summary,
          result.type_metadata_handoff.unsafe_pointer_extension_summary) &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_keyword_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .pointer_arithmetic_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .raw_pointer_type_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_operation_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .normalized_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .normalized_sites +
              result.type_metadata_handoff.unsafe_pointer_extension_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites;
  result.inline_asm_intrinsic_governance_summary =
      result.integration_surface.inline_asm_intrinsic_governance_summary;
  result.deterministic_inline_asm_intrinsic_governance_handoff =
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .deterministic &&
      result.integration_surface.inline_asm_intrinsic_governance_summary
          .deterministic &&
      IsEquivalentInlineAsmIntrinsicGovernanceSummary(
          result.integration_surface.inline_asm_intrinsic_governance_summary,
          result.type_metadata_handoff
              .inline_asm_intrinsic_governance_summary) &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .intrinsic_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .normalized_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_sites ==
          result.type_metadata_handoff.throws_propagation_summary
              .cache_invalidation_candidate_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .intrinsic_sites ==
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_operation_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          result.type_metadata_handoff.throws_propagation_summary
              .normalized_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .normalized_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .normalized_sites >=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .normalized_sites -
              result.type_metadata_handoff
                  .inline_asm_intrinsic_governance_summary
                  .inline_asm_sites ==
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .gate_blocked_sites ==
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
                  .intrinsic_sites -
              result.type_metadata_handoff
                  .inline_asm_intrinsic_governance_summary
                  .governed_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .normalized_sites +
              result.type_metadata_handoff
                  .inline_asm_intrinsic_governance_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites;
