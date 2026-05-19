          handoff.async_continuation_summary.deterministic &&
          handoff.async_continuation_summary.async_continuation_sites ==
              async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.async_keyword_sites ==
              async_continuation_summary.async_keyword_sites &&
          handoff.async_continuation_summary.async_function_sites ==
              async_continuation_summary.async_function_sites &&
          handoff.async_continuation_summary.continuation_allocation_sites ==
              async_continuation_summary.continuation_allocation_sites &&
          handoff.async_continuation_summary.continuation_resume_sites ==
              async_continuation_summary.continuation_resume_sites &&
          handoff.async_continuation_summary.continuation_suspend_sites ==
              async_continuation_summary.continuation_suspend_sites &&
          handoff.async_continuation_summary.async_state_machine_sites ==
              async_continuation_summary.async_state_machine_sites &&
          handoff.async_continuation_summary.normalized_sites ==
              async_continuation_summary.normalized_sites &&
          handoff.async_continuation_summary.gate_blocked_sites ==
              async_continuation_summary.gate_blocked_sites &&
          handoff.async_continuation_summary.contract_violation_sites ==
              async_continuation_summary.contract_violation_sites &&
          handoff.async_continuation_summary.async_keyword_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.async_function_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.continuation_allocation_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.continuation_resume_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.continuation_suspend_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.async_state_machine_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.normalized_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.gate_blocked_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.contract_violation_sites <=
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.async_continuation_summary.normalized_sites +
                  handoff.async_continuation_summary.gate_blocked_sites ==
              handoff.async_continuation_summary.async_continuation_sites &&
          handoff.await_lowering_suspension_state_lowering_summary.deterministic &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites ==
              await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_keyword_sites ==
              await_lowering_suspension_state_lowering_summary
                  .await_keyword_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_point_sites ==
              await_lowering_suspension_state_lowering_summary
                  .await_suspension_point_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_resume_sites ==
              await_lowering_suspension_state_lowering_summary
                  .await_resume_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_state_machine_sites ==
              await_lowering_suspension_state_lowering_summary
                  .await_state_machine_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_continuation_sites ==
              await_lowering_suspension_state_lowering_summary
                  .await_continuation_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .normalized_sites ==
              await_lowering_suspension_state_lowering_summary.normalized_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .gate_blocked_sites ==
              await_lowering_suspension_state_lowering_summary
                  .gate_blocked_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .contract_violation_sites ==
              await_lowering_suspension_state_lowering_summary
                  .contract_violation_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_keyword_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_point_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_resume_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_state_machine_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_point_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .await_continuation_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_point_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .normalized_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .gate_blocked_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .contract_violation_sites <=
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
          handoff.await_lowering_suspension_state_lowering_summary
                  .normalized_sites +
                  handoff.await_lowering_suspension_state_lowering_summary
                      .gate_blocked_sites ==
              handoff.await_lowering_suspension_state_lowering_summary
                  .await_suspension_sites &&
