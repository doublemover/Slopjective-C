          handoff.ns_error_bridging_summary.deterministic &&
          handoff.ns_error_bridging_summary.ns_error_bridging_sites ==
              ns_error_bridging_summary.ns_error_bridging_sites &&
          handoff.ns_error_bridging_summary.ns_error_parameter_sites ==
              ns_error_bridging_summary.ns_error_parameter_sites &&
          handoff.ns_error_bridging_summary.ns_error_out_parameter_sites ==
              ns_error_bridging_summary.ns_error_out_parameter_sites &&
          handoff.ns_error_bridging_summary.ns_error_bridge_path_sites ==
              ns_error_bridging_summary.ns_error_bridge_path_sites &&
          handoff.ns_error_bridging_summary.failable_call_sites ==
              ns_error_bridging_summary.failable_call_sites &&
          handoff.ns_error_bridging_summary.normalized_sites ==
              ns_error_bridging_summary.normalized_sites &&
          handoff.ns_error_bridging_summary.bridge_boundary_sites ==
              ns_error_bridging_summary.bridge_boundary_sites &&
          handoff.ns_error_bridging_summary.contract_violation_sites ==
              ns_error_bridging_summary.contract_violation_sites &&
          handoff.ns_error_bridging_summary.ns_error_parameter_sites <=
              handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
          handoff.ns_error_bridging_summary.ns_error_out_parameter_sites <=
              handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
          handoff.ns_error_bridging_summary.ns_error_bridge_path_sites <=
              handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
          handoff.ns_error_bridging_summary.failable_call_sites <=
              handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
          handoff.ns_error_bridging_summary.normalized_sites <=
              handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
          handoff.ns_error_bridging_summary.bridge_boundary_sites <=
              handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
          handoff.ns_error_bridging_summary.normalized_sites +
                  handoff.ns_error_bridging_summary.bridge_boundary_sites ==
              handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
          handoff.ns_error_bridging_summary.contract_violation_sites <=
              handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
          handoff.error_diagnostics_recovery_summary.deterministic &&
          handoff.error_diagnostics_recovery_summary
                  .error_diagnostics_recovery_sites ==
              error_diagnostics_recovery_summary
                  .error_diagnostics_recovery_sites &&
          handoff.error_diagnostics_recovery_summary.diagnostic_emit_sites ==
              error_diagnostics_recovery_summary.diagnostic_emit_sites &&
          handoff.error_diagnostics_recovery_summary.recovery_anchor_sites ==
              error_diagnostics_recovery_summary.recovery_anchor_sites &&
          handoff.error_diagnostics_recovery_summary.recovery_boundary_sites ==
              error_diagnostics_recovery_summary.recovery_boundary_sites &&
          handoff.error_diagnostics_recovery_summary
                  .fail_closed_diagnostic_sites ==
              error_diagnostics_recovery_summary.fail_closed_diagnostic_sites &&
          handoff.error_diagnostics_recovery_summary.normalized_sites ==
              error_diagnostics_recovery_summary.normalized_sites &&
          handoff.error_diagnostics_recovery_summary.gate_blocked_sites ==
              error_diagnostics_recovery_summary.gate_blocked_sites &&
          handoff.error_diagnostics_recovery_summary.contract_violation_sites ==
              error_diagnostics_recovery_summary.contract_violation_sites &&
          handoff.error_diagnostics_recovery_summary.diagnostic_emit_sites <=
              handoff.error_diagnostics_recovery_summary
                  .error_diagnostics_recovery_sites &&
          handoff.error_diagnostics_recovery_summary.recovery_anchor_sites <=
              handoff.error_diagnostics_recovery_summary
                  .error_diagnostics_recovery_sites &&
          handoff.error_diagnostics_recovery_summary.recovery_boundary_sites <=
              handoff.error_diagnostics_recovery_summary
                  .error_diagnostics_recovery_sites &&
          handoff.error_diagnostics_recovery_summary
                  .fail_closed_diagnostic_sites <=
              handoff.error_diagnostics_recovery_summary
                  .error_diagnostics_recovery_sites &&
          handoff.error_diagnostics_recovery_summary.normalized_sites <=
              handoff.error_diagnostics_recovery_summary
                  .error_diagnostics_recovery_sites &&
          handoff.error_diagnostics_recovery_summary.gate_blocked_sites <=
              handoff.error_diagnostics_recovery_summary
                  .error_diagnostics_recovery_sites &&
          handoff.error_diagnostics_recovery_summary.contract_violation_sites <=
              handoff.error_diagnostics_recovery_summary
                  .error_diagnostics_recovery_sites &&
          handoff.error_diagnostics_recovery_summary.normalized_sites +
                  handoff.error_diagnostics_recovery_summary.gate_blocked_sites ==
              handoff.error_diagnostics_recovery_summary
                  .error_diagnostics_recovery_sites &&
          handoff.result_like_lowering_summary.deterministic &&
          handoff.result_like_lowering_summary.result_like_sites ==
              result_like_lowering_summary.result_like_sites &&
          handoff.result_like_lowering_summary.result_success_sites ==
              result_like_lowering_summary.result_success_sites &&
          handoff.result_like_lowering_summary.result_failure_sites ==
              result_like_lowering_summary.result_failure_sites &&
          handoff.result_like_lowering_summary.result_branch_sites ==
              result_like_lowering_summary.result_branch_sites &&
          handoff.result_like_lowering_summary.result_payload_sites ==
              result_like_lowering_summary.result_payload_sites &&
          handoff.result_like_lowering_summary.normalized_sites ==
              result_like_lowering_summary.normalized_sites &&
          handoff.result_like_lowering_summary.branch_merge_sites ==
              result_like_lowering_summary.branch_merge_sites &&
          handoff.result_like_lowering_summary.contract_violation_sites ==
              result_like_lowering_summary.contract_violation_sites &&
          handoff.result_like_lowering_summary.result_success_sites <=
              handoff.result_like_lowering_summary.result_like_sites &&
          handoff.result_like_lowering_summary.result_failure_sites <=
              handoff.result_like_lowering_summary.result_like_sites &&
          handoff.result_like_lowering_summary.result_branch_sites <=
              handoff.result_like_lowering_summary.result_like_sites &&
          handoff.result_like_lowering_summary.result_payload_sites <=
              handoff.result_like_lowering_summary.result_like_sites &&
          handoff.result_like_lowering_summary.normalized_sites <=
              handoff.result_like_lowering_summary.result_like_sites &&
          handoff.result_like_lowering_summary.branch_merge_sites <=
              handoff.result_like_lowering_summary.result_like_sites &&
          handoff.result_like_lowering_summary.contract_violation_sites <=
              handoff.result_like_lowering_summary.result_like_sites &&
          handoff.result_like_lowering_summary.result_success_sites +
                  handoff.result_like_lowering_summary.result_failure_sites ==
              handoff.result_like_lowering_summary.normalized_sites &&
          handoff.result_like_lowering_summary.normalized_sites +
                  handoff.result_like_lowering_summary.branch_merge_sites ==
              handoff.result_like_lowering_summary.result_like_sites &&
