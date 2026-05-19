          handoff.task_runtime_cancellation_summary.deterministic &&
          handoff.task_runtime_cancellation_summary.task_runtime_interop_sites ==
              task_runtime_cancellation_summary.task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary.runtime_hook_sites ==
              task_runtime_cancellation_summary.runtime_hook_sites &&
          handoff.task_runtime_cancellation_summary.cancellation_check_sites ==
              task_runtime_cancellation_summary.cancellation_check_sites &&
          handoff.task_runtime_cancellation_summary.cancellation_handler_sites ==
              task_runtime_cancellation_summary.cancellation_handler_sites &&
          handoff.task_runtime_cancellation_summary.suspension_point_sites ==
              task_runtime_cancellation_summary.suspension_point_sites &&
          handoff.task_runtime_cancellation_summary
                  .cancellation_propagation_sites ==
              task_runtime_cancellation_summary
                  .cancellation_propagation_sites &&
          handoff.task_runtime_cancellation_summary.normalized_sites ==
              task_runtime_cancellation_summary.normalized_sites &&
          handoff.task_runtime_cancellation_summary.gate_blocked_sites ==
              task_runtime_cancellation_summary.gate_blocked_sites &&
          handoff.task_runtime_cancellation_summary.contract_violation_sites ==
              task_runtime_cancellation_summary.contract_violation_sites &&
          handoff.task_runtime_cancellation_summary.runtime_hook_sites <=
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary.cancellation_check_sites <=
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary.cancellation_handler_sites <=
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary.suspension_point_sites <=
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary
                  .cancellation_propagation_sites <=
              handoff.task_runtime_cancellation_summary
                  .cancellation_check_sites &&
          handoff.task_runtime_cancellation_summary
                  .cancellation_propagation_sites <=
              handoff.task_runtime_cancellation_summary
                  .cancellation_handler_sites &&
          handoff.task_runtime_cancellation_summary.normalized_sites <=
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary.gate_blocked_sites <=
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary.gate_blocked_sites <=
              handoff.task_runtime_cancellation_summary
                  .cancellation_propagation_sites &&
          handoff.task_runtime_cancellation_summary.contract_violation_sites <=
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
          handoff.task_runtime_cancellation_summary.normalized_sites +
                  handoff.task_runtime_cancellation_summary.gate_blocked_sites ==
              handoff.task_runtime_cancellation_summary
                  .task_runtime_interop_sites &&
