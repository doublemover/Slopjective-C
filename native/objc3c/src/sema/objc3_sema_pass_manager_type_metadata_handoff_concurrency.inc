  result.async_continuation_summary =
      result.integration_surface.async_continuation_summary;
  result.deterministic_async_continuation_handoff =
      result.type_metadata_handoff.async_continuation_summary.deterministic &&
      result.integration_surface.async_continuation_summary.deterministic &&
      IsEquivalentAsyncContinuationSummary(
          result.integration_surface.async_continuation_summary,
          result.type_metadata_handoff.async_continuation_summary) &&
      result.type_metadata_handoff.async_continuation_summary
              .async_keyword_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .async_function_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .continuation_allocation_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .continuation_resume_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .continuation_suspend_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .async_state_machine_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary.normalized_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary.normalized_sites +
              result.type_metadata_handoff.async_continuation_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites;
  result.await_lowering_suspension_state_lowering_summary =
      result.integration_surface.await_lowering_suspension_state_lowering_summary;
  result.deterministic_await_lowering_suspension_state_lowering_handoff =
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary.deterministic &&
      result.integration_surface
              .await_lowering_suspension_state_lowering_summary.deterministic &&
      IsEquivalentAwaitLoweringSuspensionStateSummary(
          result.integration_surface
              .await_lowering_suspension_state_lowering_summary,
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary) &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_keyword_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary.await_resume_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_state_machine_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_continuation_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary.normalized_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .contract_violation_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary.normalized_sites +
              result.type_metadata_handoff
                  .await_lowering_suspension_state_lowering_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites;
