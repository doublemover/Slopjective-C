  result.ns_error_bridging_summary =
      result.integration_surface.ns_error_bridging_summary;
  result.deterministic_ns_error_bridging_handoff =
      result.type_metadata_handoff.ns_error_bridging_summary.deterministic &&
      result.integration_surface.ns_error_bridging_summary.deterministic &&
      IsEquivalentNSErrorBridgingSummary(
          result.integration_surface.ns_error_bridging_summary,
          result.type_metadata_handoff.ns_error_bridging_summary) &&
      result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_parameter_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_out_parameter_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridge_path_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary.failable_call_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary.normalized_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary
              .bridge_boundary_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary.normalized_sites +
              result.type_metadata_handoff.ns_error_bridging_summary
                  .bridge_boundary_sites ==
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites;
  result.error_diagnostics_recovery_summary =
      result.integration_surface.error_diagnostics_recovery_summary;
  result.deterministic_error_diagnostics_recovery_handoff =
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .deterministic &&
      result.integration_surface.error_diagnostics_recovery_summary
          .deterministic &&
      IsEquivalentErrorDiagnosticsRecoverySummary(
          result.integration_surface.error_diagnostics_recovery_summary,
          result.type_metadata_handoff.error_diagnostics_recovery_summary) &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .diagnostic_emit_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .recovery_anchor_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .recovery_boundary_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .fail_closed_diagnostic_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .fail_closed_diagnostic_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .diagnostic_emit_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .normalized_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .normalized_sites +
              result.type_metadata_handoff.error_diagnostics_recovery_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites;
  result.result_like_lowering_summary =
      result.integration_surface.result_like_lowering_summary;
  result.deterministic_result_like_lowering_handoff =
      result.type_metadata_handoff.result_like_lowering_summary.deterministic &&
      result.integration_surface.result_like_lowering_summary.deterministic &&
      IsEquivalentResultLikeLoweringSummary(
          result.integration_surface.result_like_lowering_summary,
          result.type_metadata_handoff.result_like_lowering_summary) &&
      result.type_metadata_handoff.result_like_lowering_summary
              .result_success_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .result_failure_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .result_branch_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .result_payload_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary.normalized_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .branch_merge_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .result_success_sites +
              result.type_metadata_handoff.result_like_lowering_summary
                  .result_failure_sites ==
          result.type_metadata_handoff.result_like_lowering_summary
              .normalized_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .normalized_sites +
              result.type_metadata_handoff.result_like_lowering_summary
                  .branch_merge_sites ==
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites;
  result.unwind_cleanup_summary = result.integration_surface.unwind_cleanup_summary;
  result.deterministic_unwind_cleanup_handoff =
      result.type_metadata_handoff.unwind_cleanup_summary.deterministic &&
      result.integration_surface.unwind_cleanup_summary.deterministic &&
      IsEquivalentUnwindCleanupSummary(
          result.integration_surface.unwind_cleanup_summary,
          result.type_metadata_handoff.unwind_cleanup_summary) &&
      result.type_metadata_handoff.unwind_cleanup_summary.exceptional_exit_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.cleanup_action_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.cleanup_scope_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.cleanup_resume_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.normalized_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.fail_closed_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.normalized_sites +
              result.type_metadata_handoff.unwind_cleanup_summary
                  .fail_closed_sites ==
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites;
