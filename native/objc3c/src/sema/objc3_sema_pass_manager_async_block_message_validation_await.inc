  const auto &await_summary =
      surface.await_lowering_suspension_state_lowering_summary;
  record.await_lowering_suspension_state_lowering_ready =
      deterministic_await_lowering_suspension_state_lowering_handoff &&
      await_summary.await_suspension_sites ==
          surface.await_lowering_suspension_state_lowering_sites_total &&
      await_summary.await_keyword_sites ==
          surface
              .await_lowering_suspension_state_lowering_await_keyword_sites_total &&
      await_summary.await_suspension_point_sites ==
          surface
              .await_lowering_suspension_state_lowering_await_suspension_point_sites_total &&
      await_summary.await_resume_sites ==
          surface
              .await_lowering_suspension_state_lowering_await_resume_sites_total &&
      await_summary.await_state_machine_sites ==
          surface
              .await_lowering_suspension_state_lowering_await_state_machine_sites_total &&
      await_summary.await_continuation_sites ==
          surface
              .await_lowering_suspension_state_lowering_await_continuation_sites_total &&
      await_summary.normalized_sites ==
          surface
              .await_lowering_suspension_state_lowering_normalized_sites_total &&
      await_summary.gate_blocked_sites ==
          surface
              .await_lowering_suspension_state_lowering_gate_blocked_sites_total &&
      await_summary.contract_violation_sites ==
          surface
              .await_lowering_suspension_state_lowering_contract_violation_sites_total &&
      await_summary.await_keyword_sites <=
          await_summary.await_suspension_sites &&
      await_summary.await_suspension_point_sites <=
          await_summary.await_suspension_sites &&
      await_summary.await_resume_sites <=
          await_summary.await_suspension_sites &&
      await_summary.await_state_machine_sites <=
          await_summary.await_suspension_point_sites &&
      await_summary.await_continuation_sites <=
          await_summary.await_suspension_point_sites &&
      await_summary.normalized_sites <= await_summary.await_suspension_sites &&
      await_summary.gate_blocked_sites <=
          await_summary.await_suspension_sites &&
      await_summary.contract_violation_sites <=
          await_summary.await_suspension_sites &&
      await_summary.normalized_sites + await_summary.gate_blocked_sites ==
          await_summary.await_suspension_sites &&
      await_summary.deterministic;
