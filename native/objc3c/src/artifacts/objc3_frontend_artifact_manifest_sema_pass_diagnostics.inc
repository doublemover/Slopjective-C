void AppendObjc3FrontendArtifactManifestSemaPassDiagnostics(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result) {
  manifest << "      \"sema_pass_manager\": {\"diagnostics_after_build\":"
           << pipeline_result.sema_diagnostics_after_pass[0]
           << ",\"diagnostics_after_validate_bodies\":"
           << pipeline_result.sema_diagnostics_after_pass[1]
           << ",\"diagnostics_after_validate_pure_contract\":"
           << pipeline_result.sema_diagnostics_after_pass[2]
           << ",\"diagnostics_emitted_by_build\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[0]
           << ",\"diagnostics_emitted_by_validate_bodies\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[1]
           << ",\"diagnostics_emitted_by_validate_pure_contract\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[2]
           << ",\"diagnostics_monotonic\":"
           << (pipeline_result.sema_parity_surface
                       .diagnostics_after_pass_monotonic
                   ? "true"
                   : "false")
           << ",\"diagnostics_total\":"
           << pipeline_result.sema_parity_surface.diagnostics_total
           << ",\"deterministic_semantic_diagnostics\":"
           << (pipeline_result.sema_parity_surface
                       .deterministic_semantic_diagnostics
                   ? "true"
                   : "false")
           << ",\"diagnostics_accounting_consistent\":"
           << (pipeline_result.sema_parity_surface
                       .diagnostics_accounting_consistent
                   ? "true"
                   : "false")
           << ",\"diagnostics_bus_publish_consistent\":"
           << (pipeline_result.sema_parity_surface
                       .diagnostics_bus_publish_consistent
                   ? "true"
                   : "false")
           << ",\"diagnostics_canonicalized\":"
           << (pipeline_result.sema_parity_surface.diagnostics_canonicalized
                   ? "true"
                   : "false")
           << ",\"diagnostics_hardening_satisfied\":"
           << (pipeline_result.sema_parity_surface
                       .diagnostics_hardening_satisfied
                   ? "true"
                   : "false")
           << ",\"pass_flow_recovery_replay_contract_satisfied\":"
           << (pipeline_result.sema_parity_surface
                       .pass_flow_recovery_replay_contract_satisfied
                   ? "true"
                   : "false")
           << ",\"pass_flow_recovery_replay_key\":\""
           << pipeline_result.sema_parity_surface.pass_flow_recovery_replay_key
           << "\",\"pass_flow_recovery_replay_key_deterministic\":"
           << (pipeline_result.sema_parity_surface
                       .pass_flow_recovery_replay_key_deterministic
                   ? "true"
                   : "false")
           << ",\"pass_flow_recovery_determinism_hardening_satisfied\":"
           << (pipeline_result.sema_parity_surface
                       .pass_flow_recovery_determinism_hardening_satisfied
                   ? "true"
                   : "false")
           << ",\"deterministic_type_metadata_handoff\":"
           << (pipeline_result.sema_parity_surface
                       .deterministic_type_metadata_handoff
                   ? "true"
                   : "false")
           << ",\"pass_flow_configured_count\":"
           << pipeline_result.sema_pass_flow_summary.configured_pass_count
           << ",\"pass_flow_executed_count\":"
           << pipeline_result.sema_pass_flow_summary.executed_pass_count
           << ",\"pass_flow_language_profile\":\""
           << (pipeline_result.sema_pass_flow_summary.language_profile ==
                       Objc3SemaLanguageProfile::Canonical
                   ? "canonical"
                   : "canonical")
           << "\",\"pass_flow_canonical_literal_rejection_total\":"
           << pipeline_result.canonical_literal_rejection_counts
                  .total_literal_sites()
           << ",\"pass_flow_duplicate_execution_count\":"
           << pipeline_result.sema_pass_flow_summary
                  .duplicate_pass_execution_count
           << ",\"pass_flow_missing_execution_count\":"
           << pipeline_result.sema_pass_flow_summary.missing_pass_execution_count
           << ",\"pass_flow_diagnostics_total\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_total
           << ",\"pass_flow_diagnostics_emitted_by_build\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_emitted_by_pass[0]
           << ",\"pass_flow_diagnostics_emitted_by_validate_bodies\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_emitted_by_pass[1]
           << ",\"pass_flow_diagnostics_emitted_by_validate_pure_contract\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_emitted_by_pass[2]
           << ",\"pass_flow_transition_edge_count\":"
           << pipeline_result.sema_pass_flow_summary.transition_edge_count
           << ",\"pass_flow_order_matches_contract\":"
           << (pipeline_result.sema_pass_flow_summary
                       .pass_order_matches_contract
                   ? "true"
                   : "false")
           << ",\"pass_flow_diagnostics_emission_totals_consistent\":"
           << (pipeline_result.sema_pass_flow_summary
                       .diagnostics_emission_totals_consistent
                   ? "true"
                   : "false")
           << ",\"pass_flow_diagnostics_accounting_consistent\":"
           << (pipeline_result.sema_pass_flow_summary
                       .diagnostics_accounting_consistent
                   ? "true"
                   : "false")
           << ",\"pass_flow_diagnostics_bus_publish_consistent\":"
           << (pipeline_result.sema_pass_flow_summary
                       .diagnostics_bus_publish_consistent
                   ? "true"
                   : "false")
           << ",\"pass_flow_diagnostics_canonicalized\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_canonicalized
                   ? "true"
                   : "false")
           << ",\"pass_flow_diagnostics_hardening_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary
                       .diagnostics_hardening_satisfied
                   ? "true"
                   : "false")
           << ",\"pass_flow_parser_recovery_replay_ready\":"
           << (pipeline_result.sema_pass_flow_summary
                       .parser_recovery_replay_ready
                   ? "true"
                   : "false")
           << ",\"pass_flow_parser_recovery_replay_case_present\":"
           << (pipeline_result.sema_pass_flow_summary
                       .parser_recovery_replay_case_present
                   ? "true"
                   : "false")
           << ",\"pass_flow_parser_recovery_replay_case_passed\":"
           << (pipeline_result.sema_pass_flow_summary
                       .parser_recovery_replay_case_passed
                   ? "true"
                   : "false")
           << ",\"pass_flow_recovery_replay_contract_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary
                       .recovery_replay_contract_satisfied
                   ? "true"
                   : "false")
           << ",\"pass_flow_recovery_replay_key\":\""
           << pipeline_result.sema_pass_flow_summary.recovery_replay_key
           << "\",\"pass_flow_recovery_replay_key_deterministic\":"
           << (pipeline_result.sema_pass_flow_summary
                       .recovery_replay_key_deterministic
                   ? "true"
                   : "false")
           << ",\"pass_flow_recovery_determinism_hardening_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary
                       .recovery_determinism_hardening_satisfied
                   ? "true"
                   : "false")
           << ",\"pass_flow_compatibility_handoff_consistent\":"
           << (pipeline_result.sema_pass_flow_summary
                       .compatibility_handoff_consistent
                   ? "true"
                   : "false")
           << ",\"pass_flow_robustness_guardrails_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary
                       .robustness_guardrails_satisfied
                   ? "true"
                   : "false")
           << ",\"pass_flow_symbol_counts_consistent\":"
           << (pipeline_result.sema_pass_flow_summary
                       .symbol_flow_counts_consistent
                   ? "true"
                   : "false")
           << ",\"pass_flow_fingerprint\":"
           << pipeline_result.sema_pass_flow_summary.pass_execution_fingerprint
           << ",\"pass_flow_deterministic_handoff_key\":\""
           << pipeline_result.sema_pass_flow_summary.deterministic_handoff_key
           << "\",\"pass_flow_replay_key_deterministic\":"
           << (pipeline_result.sema_pass_flow_summary.replay_key_deterministic
                   ? "true"
                   : "false")
           << ",\"pass_flow_deterministic\":"
           << (pipeline_result.sema_pass_flow_summary.deterministic ? "true"
                                                                    : "false");
}
