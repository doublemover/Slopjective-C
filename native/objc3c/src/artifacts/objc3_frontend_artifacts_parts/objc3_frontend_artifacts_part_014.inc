           << pipeline_result.parser_contract_snapshot.long_tail_grammar_construct_count
           << ",\"long_tail_grammar_covered_constructs\":"
           << pipeline_result.parser_contract_snapshot.long_tail_grammar_covered_construct_count
           << ",\"long_tail_grammar_fingerprint\":"
           << pipeline_result.parser_contract_snapshot.long_tail_grammar_fingerprint
           << ",\"long_tail_grammar_handoff_key\":\""
           << pipeline_result.parser_contract_snapshot.long_tail_grammar_handoff_key
           << "\",\"long_tail_grammar_deterministic\":"
           << (pipeline_result.parser_contract_snapshot.long_tail_grammar_handoff_deterministic ? "true" : "false")
           << ",\"diagnostic_code_count\":"
           << parser_diag_code_coverage.unique_code_count
           << ",\"diagnostic_code_fingerprint\":"
           << parser_diag_code_coverage.unique_code_fingerprint
           << ",\"diagnostic_code_surface_deterministic\":"
           << (parser_diag_code_coverage.deterministic_surface ? "true" : "false")
           << ",\"deterministic_handoff\":"
           << (pipeline_result.parser_contract_snapshot.deterministic_handoff ? "true" : "false")
           << ",\"recovery_replay_ready\":"
           << (pipeline_result.parser_contract_snapshot.parser_recovery_replay_ready ? "true" : "false") << "},\n";
  manifest << "        \"semantic\": {\"diagnostics\":" << bundle.stage_diagnostics.semantic.size()
           << "}\n";
  manifest << "      },\n";
  manifest << "      \"parse_lowering_readiness\": {\"ready_for_lowering\": "
           << (bundle.parse_lowering_readiness_surface.ready_for_lowering ? "true" : "false")
           << ",\"parser_contract_snapshot_present\": "
           << (bundle.parse_lowering_readiness_surface.parser_contract_snapshot_present ? "true" : "false")
           << ",\"long_tail_grammar_core_feature_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_core_feature_consistent ? "true" : "false")
           << ",\"long_tail_grammar_handoff_key_deterministic\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_handoff_key_deterministic ? "true"
                                                                                                     : "false")
           << ",\"long_tail_grammar_expansion_accounting_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_expansion_accounting_consistent ? "true"
                                                                                                           : "false")
           << ",\"long_tail_grammar_replay_keys_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_replay_keys_ready ? "true" : "false")
           << ",\"long_tail_grammar_expansion_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_expansion_ready ? "true" : "false")
           << ",\"long_tail_grammar_compatibility_handoff_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_compatibility_handoff_ready ? "true"
                                                                                                       : "false")
           << ",\"long_tail_grammar_edge_case_compatibility_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_compatibility_consistent ? "true"
                                                                                                              : "false")
           << ",\"long_tail_grammar_edge_case_compatibility_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_compatibility_ready ? "true"
                                                                                                         : "false")
           << ",\"long_tail_grammar_edge_case_expansion_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_expansion_consistent ? "true"
                                                                                                          : "false")
           << ",\"long_tail_grammar_edge_case_robustness_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_robustness_ready ? "true" : "false")
           << ",\"long_tail_grammar_diagnostics_hardening_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_diagnostics_hardening_consistent ? "true"
                                                                                                            : "false")
           << ",\"long_tail_grammar_diagnostics_hardening_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_diagnostics_hardening_ready ? "true"
                                                                                                       : "false")
           << ",\"long_tail_grammar_recovery_determinism_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_recovery_determinism_consistent ? "true"
                                                                                                           : "false")
           << ",\"long_tail_grammar_recovery_determinism_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_recovery_determinism_ready ? "true"
                                                                                                      : "false")
           << ",\"long_tail_grammar_conformance_matrix_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_conformance_matrix_consistent ? "true"
                                                                                                         : "false")
           << ",\"long_tail_grammar_conformance_matrix_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_conformance_matrix_ready ? "true"
                                                                                                    : "false")
           << ",\"long_tail_grammar_integration_closeout_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_integration_closeout_consistent ? "true"
                                                                                                           : "false")
           << ",\"long_tail_grammar_gate_signoff_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_gate_signoff_ready ? "true" : "false")
           << ",\"parse_artifact_handoff_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_handoff_consistent ? "true" : "false")
           << ",\"parse_artifact_handoff_deterministic\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_handoff_deterministic ? "true" : "false")
           << ",\"parser_token_count_budget_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parser_token_count_budget_consistent ? "true" : "false")
           << ",\"parse_artifact_layout_fingerprint_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_layout_fingerprint_consistent ? "true" : "false")
           << ",\"parse_artifact_fingerprint_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_fingerprint_consistent ? "true" : "false")
           << ",\"compatibility_handoff_consistent\": "
           << (bundle.parse_lowering_readiness_surface.compatibility_handoff_consistent ? "true" : "false")
           << ",\"parser_diagnostic_surface_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parser_diagnostic_surface_consistent ? "true" : "false")
           << ",\"parser_diagnostic_code_surface_deterministic\": "
           << (bundle.parse_lowering_readiness_surface.parser_diagnostic_code_surface_deterministic ? "true"
                                                                                                     : "false")
           << ",\"language_version_pragma_coordinate_order_consistent\": "
           << (bundle.parse_lowering_readiness_surface.language_version_pragma_coordinate_order_consistent ? "true"
                                                                                                            : "false")
           << ",\"parse_artifact_replay_key_deterministic\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_replay_key_deterministic ? "true" : "false")
           << ",\"parse_artifact_diagnostics_hardening_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_diagnostics_hardening_consistent ? "true"
                                                                                                         : "false")
           << ",\"parse_artifact_edge_case_robustness_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_edge_case_robustness_consistent ? "true"
                                                                                                        : "false")
           << ",\"parse_recovery_determinism_hardening_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_recovery_determinism_hardening_consistent ? "true"
                                                                                                        : "false")
           << ",\"parser_diagnostic_grammar_hooks_recovery_determinism_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_recovery_determinism_consistent
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_recovery_determinism_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_recovery_determinism_ready
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_matrix_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_conformance_matrix_consistent
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_matrix_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_conformance_matrix_ready
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_corpus_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_conformance_corpus_consistent
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_corpus_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_conformance_corpus_ready
                   ? "true"
                   : "false")
           << ",\"parse_lowering_conformance_matrix_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_lowering_conformance_matrix_consistent ? "true"
                                                                                                    : "false")
           << ",\"parse_lowering_conformance_corpus_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_lowering_conformance_corpus_consistent ? "true"
                                                                                                      : "false")
           << ",\"parse_lowering_performance_quality_guardrails_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_lowering_performance_quality_guardrails_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_cross_lane_integration_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_cross_lane_integration_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_cross_lane_integration_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_cross_lane_integration_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_docs_runbook_sync_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_docs_runbook_sync_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_docs_runbook_sync_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_docs_runbook_sync_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_core_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_core_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_edge_compatibility_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_edge_compatibility_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_diagnostics_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_diagnostics_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_diagnostics_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_diagnostics_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_conformance_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_conformance_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_conformance_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_conformance_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_integration_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_integration_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_integration_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_integration_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_performance_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_performance_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_performance_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_performance_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_shard2_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_core_shard2_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_shard2_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_core_shard2_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_integration_closeout_signoff_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_integration_closeout_signoff_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_integration_closeout_signoff_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_integration_closeout_signoff_ready
                   ? "true"
                   : "false")
           << ",\"semantic_integration_surface_built\": "
           << (bundle.parse_lowering_readiness_surface.semantic_integration_surface_built ? "true" : "false")
           << ",\"executable_metadata_lowering_handoff_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .executable_metadata_lowering_handoff_ready
                   ? "true"
                   : "false")
           << ",\"executable_metadata_lowering_handoff_deterministic\": "
           << (bundle.parse_lowering_readiness_surface
                       .executable_metadata_lowering_handoff_deterministic
                   ? "true"
                   : "false")
           << ",\"executable_metadata_typed_lowering_handoff_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .executable_metadata_typed_lowering_handoff_ready
                   ? "true"
                   : "false")
           << ",\"executable_metadata_typed_lowering_handoff_deterministic\": "
           << (bundle.parse_lowering_readiness_surface
                       .executable_metadata_typed_lowering_handoff_deterministic
                   ? "true"
                   : "false")
           << ",\"lowering_boundary_ready\": "
           << (bundle.parse_lowering_readiness_surface.lowering_boundary_ready ? "true" : "false")
           << ",\"parse_lowering_conformance_matrix_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_matrix_case_count
           << ",\"parse_lowering_conformance_corpus_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_corpus_case_count
           << ",\"parse_lowering_conformance_corpus_passed_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_corpus_passed_case_count
           << ",\"parse_lowering_conformance_corpus_failed_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_corpus_failed_case_count
           << ",\"parse_lowering_performance_quality_guardrails_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_performance_quality_guardrails_case_count
           << ",\"parse_lowering_performance_quality_guardrails_passed_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_performance_quality_guardrails_passed_case_count
           << ",\"parse_lowering_performance_quality_guardrails_failed_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_performance_quality_guardrails_failed_case_count
           << ",\"parser_diagnostic_code_count\": "
           << bundle.parse_lowering_readiness_surface.parser_diagnostic_code_count
           << ",\"long_tail_grammar_construct_count\": "
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_construct_count
           << ",\"long_tail_grammar_covered_construct_count\": "
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_covered_construct_count
           << ",\"parser_diagnostic_code_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.parser_diagnostic_code_fingerprint
           << ",\"long_tail_grammar_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_fingerprint
           << ",\"parser_contract_snapshot_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.parser_contract_snapshot_fingerprint
           << ",\"parser_ast_shape_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.parser_ast_shape_fingerprint
           << ",\"parser_ast_top_level_layout_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.parser_ast_top_level_layout_fingerprint
           << ",\"ast_shape_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.ast_shape_fingerprint
           << ",\"ast_top_level_layout_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.ast_top_level_layout_fingerprint
           << ",\"parse_artifact_handoff_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_artifact_handoff_key
           << "\",\"long_tail_grammar_handoff_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_handoff_key
           << "\",\"long_tail_grammar_expansion_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_expansion_key
           << "\",\"long_tail_grammar_edge_case_compatibility_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_compatibility_key
           << "\",\"long_tail_grammar_edge_case_robustness_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_robustness_key
           << "\",\"long_tail_grammar_diagnostics_hardening_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_diagnostics_hardening_key
           << "\",\"long_tail_grammar_recovery_determinism_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_recovery_determinism_key
           << "\",\"long_tail_grammar_conformance_matrix_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_conformance_matrix_key
           << "\",\"long_tail_grammar_integration_closeout_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_integration_closeout_key
           << "\",\"compatibility_handoff_key\":\""
           << bundle.parse_lowering_readiness_surface.compatibility_handoff_key
           << "\",\"parse_artifact_replay_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_artifact_replay_key
           << "\",\"parse_artifact_diagnostics_hardening_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_artifact_diagnostics_hardening_key
           << "\",\"parse_artifact_edge_robustness_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_artifact_edge_robustness_key
           << "\",\"parse_recovery_determinism_hardening_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_recovery_determinism_hardening_key
           << "\",\"parser_diagnostic_grammar_hooks_recovery_determinism_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .parser_diagnostic_grammar_hooks_recovery_determinism_key
           << "\",\"parser_diagnostic_grammar_hooks_conformance_matrix_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .parser_diagnostic_grammar_hooks_conformance_matrix_key
           << "\",\"parser_diagnostic_grammar_hooks_conformance_corpus_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .parser_diagnostic_grammar_hooks_conformance_corpus_key
           << "\",\"parse_lowering_conformance_matrix_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_matrix_key
           << "\",\"parse_lowering_conformance_corpus_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_corpus_key
           << "\",\"parse_lowering_performance_quality_guardrails_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_lowering_performance_quality_guardrails_key
           << "\",\"toolchain_runtime_ga_operations_cross_lane_integration_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .toolchain_runtime_ga_operations_cross_lane_integration_key
           << "\",\"toolchain_runtime_ga_operations_docs_runbook_sync_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_docs_runbook_sync_key
           << "\",\"toolchain_runtime_ga_operations_advanced_core_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_core_key
           << "\",\"toolchain_runtime_ga_operations_advanced_edge_compatibility_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_key
           << "\",\"toolchain_runtime_ga_operations_advanced_diagnostics_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_diagnostics_key
           << "\",\"toolchain_runtime_ga_operations_advanced_conformance_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_conformance_key
           << "\",\"toolchain_runtime_ga_operations_advanced_integration_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_integration_key
           << "\",\"toolchain_runtime_ga_operations_advanced_performance_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_performance_key
           << "\",\"toolchain_runtime_ga_operations_advanced_core_shard2_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_core_shard2_key
           << "\",\"toolchain_runtime_ga_operations_integration_closeout_signoff_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .toolchain_runtime_ga_operations_integration_closeout_signoff_key
           << "\",\"executable_metadata_lowering_handoff_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .executable_metadata_lowering_handoff_key
           << "\",\"executable_metadata_typed_lowering_handoff_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .executable_metadata_typed_lowering_handoff_key
           << "\",\"failure_reason\":\"" << bundle.parse_lowering_readiness_surface.failure_reason
           << "\",\"lowering_boundary_replay_key\":\""
           << bundle.parse_lowering_readiness_surface.lowering_boundary_replay_key
           << "\"},\n";
  manifest << "      \"sema_pass_manager\": {\"diagnostics_after_build\":"
           << pipeline_result.sema_diagnostics_after_pass[0] << ",\"diagnostics_after_validate_bodies\":"
           << pipeline_result.sema_diagnostics_after_pass[1] << ",\"diagnostics_after_validate_pure_contract\":"
           << pipeline_result.sema_diagnostics_after_pass[2] << ",\"diagnostics_emitted_by_build\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[0]
           << ",\"diagnostics_emitted_by_validate_bodies\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[1]
           << ",\"diagnostics_emitted_by_validate_pure_contract\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[2] << ",\"diagnostics_monotonic\":"
           << (pipeline_result.sema_parity_surface.diagnostics_after_pass_monotonic ? "true" : "false")
           << ",\"diagnostics_total\":"
           << pipeline_result.sema_parity_surface.diagnostics_total
           << ",\"deterministic_semantic_diagnostics\":"
           << (pipeline_result.sema_parity_surface.deterministic_semantic_diagnostics ? "true" : "false")
           << ",\"diagnostics_accounting_consistent\":"
           << (pipeline_result.sema_parity_surface.diagnostics_accounting_consistent ? "true" : "false")
           << ",\"diagnostics_bus_publish_consistent\":"
           << (pipeline_result.sema_parity_surface.diagnostics_bus_publish_consistent ? "true" : "false")
           << ",\"diagnostics_canonicalized\":"
           << (pipeline_result.sema_parity_surface.diagnostics_canonicalized ? "true" : "false")
           << ",\"diagnostics_hardening_satisfied\":"
           << (pipeline_result.sema_parity_surface.diagnostics_hardening_satisfied ? "true" : "false")
           << ",\"pass_flow_recovery_replay_contract_satisfied\":"
           << (pipeline_result.sema_parity_surface.pass_flow_recovery_replay_contract_satisfied ? "true" : "false")
           << ",\"pass_flow_recovery_replay_key\":\""
           << pipeline_result.sema_parity_surface.pass_flow_recovery_replay_key
           << "\",\"pass_flow_recovery_replay_key_deterministic\":"
           << (pipeline_result.sema_parity_surface.pass_flow_recovery_replay_key_deterministic ? "true" : "false")
           << ",\"pass_flow_recovery_determinism_hardening_satisfied\":"
           << (pipeline_result.sema_parity_surface.pass_flow_recovery_determinism_hardening_satisfied ? "true" : "false")
           << ",\"deterministic_type_metadata_handoff\":"
           << (pipeline_result.sema_parity_surface.deterministic_type_metadata_handoff ? "true" : "false")
           << ",\"pass_flow_configured_count\":"
           << pipeline_result.sema_pass_flow_summary.configured_pass_count
           << ",\"pass_flow_executed_count\":"
           << pipeline_result.sema_pass_flow_summary.executed_pass_count
           << ",\"pass_flow_language_profile\":\""
           << (pipeline_result.sema_pass_flow_summary.language_profile == Objc3SemaLanguageProfile::Canonical
                   ? "canonical"
                   : "canonical")
           << "\",\"pass_flow_migration_legacy_literal_total\":"
           << pipeline_result.sema_pass_flow_summary.migration_legacy_literal_total
           << ",\"pass_flow_duplicate_execution_count\":"
           << pipeline_result.sema_pass_flow_summary.duplicate_pass_execution_count
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
           << (pipeline_result.sema_pass_flow_summary.pass_order_matches_contract ? "true" : "false")
           << ",\"pass_flow_diagnostics_emission_totals_consistent\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_emission_totals_consistent ? "true" : "false")
           << ",\"pass_flow_diagnostics_accounting_consistent\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_accounting_consistent ? "true" : "false")
           << ",\"pass_flow_diagnostics_bus_publish_consistent\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_bus_publish_consistent ? "true" : "false")
           << ",\"pass_flow_diagnostics_canonicalized\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_canonicalized ? "true" : "false")
           << ",\"pass_flow_diagnostics_hardening_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_hardening_satisfied ? "true" : "false")
           << ",\"pass_flow_parser_recovery_replay_ready\":"
           << (pipeline_result.sema_pass_flow_summary.parser_recovery_replay_ready ? "true" : "false")
           << ",\"pass_flow_parser_recovery_replay_case_present\":"
           << (pipeline_result.sema_pass_flow_summary.parser_recovery_replay_case_present ? "true" : "false")
           << ",\"pass_flow_parser_recovery_replay_case_passed\":"
           << (pipeline_result.sema_pass_flow_summary.parser_recovery_replay_case_passed ? "true" : "false")
           << ",\"pass_flow_recovery_replay_contract_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary.recovery_replay_contract_satisfied ? "true" : "false")
           << ",\"pass_flow_recovery_replay_key\":\""
           << pipeline_result.sema_pass_flow_summary.recovery_replay_key
           << "\",\"pass_flow_recovery_replay_key_deterministic\":"
           << (pipeline_result.sema_pass_flow_summary.recovery_replay_key_deterministic ? "true" : "false")
           << ",\"pass_flow_recovery_determinism_hardening_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary.recovery_determinism_hardening_satisfied ? "true" : "false")
           << ",\"pass_flow_compatibility_handoff_consistent\":"
           << (pipeline_result.sema_pass_flow_summary.compatibility_handoff_consistent ? "true" : "false")
           << ",\"pass_flow_robustness_guardrails_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary.robustness_guardrails_satisfied ? "true" : "false")
           << ",\"pass_flow_symbol_counts_consistent\":"
           << (pipeline_result.sema_pass_flow_summary.symbol_flow_counts_consistent ? "true" : "false")
           << ",\"pass_flow_fingerprint\":"
           << pipeline_result.sema_pass_flow_summary.pass_execution_fingerprint
           << ",\"pass_flow_deterministic_handoff_key\":\""
           << pipeline_result.sema_pass_flow_summary.deterministic_handoff_key
           << "\",\"pass_flow_replay_key_deterministic\":"
           << (pipeline_result.sema_pass_flow_summary.replay_key_deterministic ? "true" : "false")
           << ",\"pass_flow_deterministic\":"
           << (pipeline_result.sema_pass_flow_summary.deterministic ? "true" : "false")
           << ",\"deterministic_atomic_memory_order_mapping\":"
           << (pipeline_result.sema_parity_surface.deterministic_atomic_memory_order_mapping ? "true" : "false")
           << ",\"atomic_memory_order_mapping_total\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.total()
           << ",\"atomic_relaxed_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.relaxed
           << ",\"atomic_acquire_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.acquire
           << ",\"atomic_release_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.release
           << ",\"atomic_acq_rel_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.acq_rel
           << ",\"atomic_seq_cst_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.seq_cst
           << ",\"atomic_unmapped_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.unsupported
           << ",\"deterministic_vector_type_lowering\":"
           << (pipeline_result.sema_parity_surface.deterministic_vector_type_lowering ? "true" : "false")
           << ",\"vector_type_lowering_total\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.total()
           << ",\"vector_return_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.return_annotations
           << ",\"vector_param_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.param_annotations
           << ",\"vector_i32_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.i32_annotations
           << ",\"vector_bool_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.bool_annotations
           << ",\"vector_lane2_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane2_annotations
           << ",\"vector_lane4_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane4_annotations
           << ",\"vector_lane8_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane8_annotations
           << ",\"vector_lane16_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane16_annotations
           << ",\"vector_unsupported_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.unsupported_annotations
           << ",\"ready\":"
           << (pipeline_result.sema_parity_surface.ready ? "true" : "false")
           << ",\"parity_ready\":"
           << (IsReadyObjc3SemaParityContractSurface(pipeline_result.sema_parity_surface) ? "true" : "false")
           << ",\"globals_total\":"
           << pipeline_result.sema_parity_surface.globals_total
           << ",\"functions_total\":"
           << pipeline_result.sema_parity_surface.functions_total
           << ",\"type_metadata_global_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_global_entries
           << ",\"type_metadata_function_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_function_entries
           // Legacy extraction anchor retained for contract tests:
           // << pipeline_result.sema_parity_surface.type_metadata_function_entries << "},\n";
           << ",\"deterministic_interface_implementation_handoff\":"
           << (pipeline_result.sema_parity_surface.deterministic_interface_implementation_handoff ? "true" : "false")
           << ",\"interfaces_total\":"
           << pipeline_result.sema_parity_surface.interfaces_total
           << ",\"implementations_total\":"
           << pipeline_result.sema_parity_surface.implementations_total
           << ",\"type_metadata_interface_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_interface_entries
           << ",\"type_metadata_implementation_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_implementation_entries
           << ",\"declared_interfaces\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.declared_interfaces
           << ",\"declared_implementations\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.declared_implementations
           << ",\"resolved_interfaces\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.resolved_interfaces
           << ",\"resolved_implementations\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.resolved_implementations
           << ",\"interface_method_symbols_total\":"
           << pipeline_result.sema_parity_surface.interface_method_symbols_total
           << ",\"implementation_method_symbols_total\":"
           << pipeline_result.sema_parity_surface.implementation_method_symbols_total
           << ",\"linked_implementation_symbols_total\":"
           << pipeline_result.sema_parity_surface.linked_implementation_symbols_total
           << ",\"deterministic_interface_implementation_summary\":"
           << (pipeline_result.sema_parity_surface.interface_implementation_summary.deterministic ? "true" : "false")
           << ",\"deterministic_protocol_category_handoff\":"
           << (protocol_category_summary.deterministic_protocol_category_handoff ? "true" : "false")
           << ",\"type_metadata_protocol_entries\":"
           << protocol_category_summary.resolved_protocol_symbols
           << ",\"type_metadata_category_entries\":"
           << protocol_category_summary.resolved_category_symbols
           << ",\"deterministic_class_protocol_category_linking_handoff\":"
           << (class_protocol_category_linking_summary.deterministic_class_protocol_category_linking_handoff
                   ? "true"
                   : "false")
           << ",\"class_protocol_category_declared_class_interfaces\":"
           << class_protocol_category_linking_summary.declared_class_interfaces
           << ",\"class_protocol_category_declared_class_implementations\":"
           << class_protocol_category_linking_summary.declared_class_implementations
           << ",\"class_protocol_category_resolved_class_interfaces\":"
           << class_protocol_category_linking_summary.resolved_class_interfaces
           << ",\"class_protocol_category_resolved_class_implementations\":"
           << class_protocol_category_linking_summary.resolved_class_implementations
           << ",\"class_protocol_category_linked_class_method_symbols\":"
           << class_protocol_category_linking_summary.linked_class_method_symbols
           << ",\"class_protocol_category_linked_category_method_symbols\":"
           << class_protocol_category_linking_summary.linked_category_method_symbols
           << ",\"class_protocol_category_protocol_composition_sites\":"
           << class_protocol_category_linking_summary.protocol_composition_sites
           << ",\"class_protocol_category_protocol_composition_symbols\":"
           << class_protocol_category_linking_summary.protocol_composition_symbols
           << ",\"class_protocol_category_category_composition_sites\":"
           << class_protocol_category_linking_summary.category_composition_sites
           << ",\"class_protocol_category_category_composition_symbols\":"
           << class_protocol_category_linking_summary.category_composition_symbols
           << ",\"class_protocol_category_invalid_protocol_composition_sites\":"
           << class_protocol_category_linking_summary.invalid_protocol_composition_sites
           << ",\"deterministic_selector_normalization_handoff\":"
           << (selector_normalization_summary.deterministic_selector_normalization_handoff ? "true" : "false")
           << ",\"selector_method_declaration_entries\":"
           << selector_normalization_summary.method_declaration_entries
           << ",\"selector_normalized_method_declarations\":"
           << selector_normalization_summary.normalized_method_declarations
           << ",\"selector_piece_entries\":"
           << selector_normalization_summary.selector_piece_entries
           << ",\"selector_piece_parameter_links\":"
           << selector_normalization_summary.selector_piece_parameter_links
           << ",\"deterministic_property_attribute_handoff\":"
           << (property_attribute_summary.deterministic_property_attribute_handoff ? "true" : "false")
           << ",\"property_declaration_entries\":"
           << property_attribute_summary.property_declaration_entries
           << ",\"property_attribute_entries\":"
           << property_attribute_summary.property_attribute_entries
           << ",\"property_attribute_value_entries\":"
           << property_attribute_summary.property_attribute_value_entries
           << ",\"property_accessor_modifier_entries\":"
           << property_attribute_summary.property_accessor_modifier_entries
           << ",\"property_getter_selector_entries\":"
           << property_attribute_summary.property_getter_selector_entries
           << ",\"property_setter_selector_entries\":"
           << property_attribute_summary.property_setter_selector_entries
           << ",\"runtime_metadata_source_ownership_contract_id\":\""
           << runtime_metadata_source_ownership.contract_id
           << "\",\"runtime_metadata_source_schema\":\""
           << runtime_metadata_source_ownership.canonical_source_schema
           << "\",\"runtime_metadata_ivar_source_model\":\""
           << runtime_metadata_source_ownership.ivar_record_source_model
           << "\",\"frontend_owns_runtime_metadata_source_records\":"
           << (runtime_metadata_source_ownership.frontend_owns_runtime_metadata_source_records ? "true" : "false")
           << ",\"runtime_metadata_source_records_ready_for_lowering\":"
           << (runtime_metadata_source_ownership.runtime_metadata_source_records_ready_for_lowering ? "true"
                                                                                                   : "false")
           << ",\"native_runtime_library_present\":"
           << (runtime_metadata_source_ownership.native_runtime_library_present ? "true" : "false")
           << ",\"runtime_link_test_only\":"
           << (runtime_metadata_source_ownership.runtime_link_test_only ? "true" : "false")
           << ",\"runtime_metadata_source_boundary_fail_closed\":"
           << (runtime_metadata_source_ownership.fail_closed ? "true" : "false")
           << ",\"runtime_metadata_source_boundary_ready\":"
           << (IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(runtime_metadata_source_ownership) ? "true"
                                                                                                      : "false")
           << ",\"deterministic_runtime_metadata_source_schema\":"
           << (runtime_metadata_source_ownership.deterministic_source_schema ? "true" : "false")
           << ",\"runtime_metadata_class_record_count\":"
           << runtime_metadata_source_ownership.class_record_count
           << ",\"runtime_metadata_protocol_record_count\":"
           << runtime_metadata_source_ownership.protocol_record_count
           << ",\"runtime_metadata_category_interface_record_count\":"
           << runtime_metadata_source_ownership.category_interface_record_count
           << ",\"runtime_metadata_category_implementation_record_count\":"
           << runtime_metadata_source_ownership.category_implementation_record_count
           << ",\"runtime_metadata_property_record_count\":"
           << runtime_metadata_source_ownership.property_record_count
           << ",\"runtime_metadata_method_record_count\":"
           << runtime_metadata_source_ownership.method_record_count
           << ",\"runtime_metadata_ivar_record_count\":"
           << runtime_metadata_source_ownership.ivar_record_count
           << ",\"runtime_metadata_source_boundary_failure_reason\":\""
           << runtime_metadata_source_ownership.failure_reason
           << "\""
           << ",\"runtime_export_legality_contract_id\":\""
           << runtime_export_legality.contract_id
           << "\",\"runtime_export_semantic_boundary_frozen\":"
           << (runtime_export_legality.semantic_boundary_frozen ? "true" : "false")
           << ",\"runtime_export_metadata_export_enforcement_ready\":"
           << (runtime_export_legality.metadata_export_enforcement_ready ? "true" : "false")
           << ",\"runtime_export_fail_closed\":"
           << (runtime_export_legality.fail_closed ? "true" : "false")
           << ",\"runtime_export_boundary_ready\":"
           << (IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality) ? "true"
                                                                                 : "false")
           << ",\"runtime_export_duplicate_runtime_identity_enforcement_pending\":"
           << (runtime_export_legality.duplicate_runtime_identity_enforcement_pending ? "true"
                                                                                    : "false")
           << ",\"runtime_export_incomplete_declaration_export_blocking_pending\":"
           << (runtime_export_legality.incomplete_declaration_export_blocking_pending ? "true"
                                                                                    : "false")
           << ",\"runtime_export_illegal_redeclaration_mix_export_blocking_pending\":"
           << (runtime_export_legality.illegal_redeclaration_mix_export_blocking_pending ? "true"
                                                                                        : "false")
           << ",\"runtime_export_class_record_count\":"
           << runtime_export_legality.class_record_count
           << ",\"runtime_export_protocol_record_count\":"
           << runtime_export_legality.protocol_record_count
           << ",\"runtime_export_category_record_count\":"
           << runtime_export_legality.category_record_count
           << ",\"runtime_export_property_record_count\":"
           << runtime_export_legality.property_record_count
           << ",\"runtime_export_method_record_count\":"
           << runtime_export_legality.method_record_count
           << ",\"runtime_export_ivar_record_count\":"
           << runtime_export_legality.ivar_record_count
           << ",\"runtime_export_invalid_protocol_composition_sites\":"
           << runtime_export_legality.invalid_protocol_composition_sites
           << ",\"runtime_export_property_attribute_invalid_entries\":"
           << runtime_export_legality.property_attribute_invalid_entries
           << ",\"runtime_export_property_attribute_contract_violations\":"
           << runtime_export_legality.property_attribute_contract_violations
           << ",\"runtime_export_invalid_type_annotation_sites\":"
           << runtime_export_legality.invalid_type_annotation_sites
           << ",\"runtime_export_property_ivar_binding_missing\":"
           << runtime_export_legality.property_ivar_binding_missing
           << ",\"runtime_export_property_ivar_binding_conflicts\":"
           << runtime_export_legality.property_ivar_binding_conflicts
           << ",\"runtime_export_implementation_resolution_misses\":"
           << runtime_export_legality.implementation_resolution_misses
           << ",\"runtime_export_method_resolution_misses\":"
           << runtime_export_legality.method_resolution_misses
           << ",\"runtime_export_failure_reason\":\""
           << runtime_export_legality.failure_reason
           << "\""
           << ",\"runtime_export_enforcement_contract_id\":\""
           << runtime_export_enforcement.contract_id
           << "\",\"runtime_export_metadata_completeness_enforced\":"
           << (runtime_export_enforcement.metadata_completeness_enforced ? "true"
                                                                        : "false")
           << ",\"runtime_export_duplicate_runtime_identity_suppression_enforced\":"
           << (runtime_export_enforcement
                           .duplicate_runtime_identity_suppression_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_illegal_redeclaration_mix_blocking_enforced\":"
           << (runtime_export_enforcement
                           .illegal_redeclaration_mix_blocking_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_metadata_shape_drift_blocking_enforced\":"
           << (runtime_export_enforcement
                           .metadata_shape_drift_blocking_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_enforcement_fail_closed\":"
           << (runtime_export_enforcement.fail_closed ? "true" : "false")
           << ",\"runtime_export_ready_for_runtime_export\":"
           << (runtime_export_enforcement.ready_for_runtime_export ? "true"
                                                                  : "false")
           // diagnostic precision anchor: manifest evidence must
           // preserve the duplicate/incomplete/illegal counters that feed the
           // precise category-attachment, duplicate-member, and ambiguity
           // diagnostics emitted by the runtime metadata blocker.
           << ",\"runtime_export_duplicate_runtime_identity_sites\":"
           << runtime_export_enforcement.duplicate_runtime_identity_sites
           << ",\"runtime_export_incomplete_declaration_sites\":"
           << runtime_export_enforcement.incomplete_declaration_sites
           << ",\"runtime_export_illegal_redeclaration_mix_sites\":"
           << runtime_export_enforcement.illegal_redeclaration_mix_sites
           << ",\"runtime_export_metadata_shape_drift_sites\":"
           << runtime_export_enforcement.metadata_shape_drift_sites
           << ",\"runtime_export_enforcement_failure_reason\":\""
           << runtime_export_enforcement.failure_reason
           << "\""
           << ",\"runtime_metadata_section_abi_contract_id\":\""
           << runtime_metadata_section_abi.contract_id
           << "\",\"runtime_metadata_section_boundary_frozen\":"
           << (runtime_metadata_section_abi.boundary_frozen ? "true" : "false")
           << ",\"runtime_metadata_section_fail_closed\":"
           << (runtime_metadata_section_abi.fail_closed ? "true" : "false")
           << ",\"runtime_metadata_section_object_file_inventory_frozen\":"
           << (runtime_metadata_section_abi.object_file_section_inventory_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_metadata_section_symbol_policy_frozen\":"
           << (runtime_metadata_section_abi.symbol_policy_frozen ? "true"
                                                                : "false")
           << ",\"runtime_metadata_section_visibility_model_frozen\":"
           << (runtime_metadata_section_abi.visibility_model_frozen ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_section_retention_policy_frozen\":"
           << (runtime_metadata_section_abi.retention_policy_frozen ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_section_ready_for_scaffold\":"
           << (runtime_metadata_section_abi.ready_for_section_scaffold ? "true"
                                                                       : "false")
           << ",\"runtime_metadata_section_logical_image_info_section\":\""
           << runtime_metadata_section_abi.logical_image_info_section
           << "\",\"runtime_metadata_section_logical_class_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_class_descriptor_section
           << "\",\"runtime_metadata_section_logical_protocol_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_protocol_descriptor_section
           << "\",\"runtime_metadata_section_logical_category_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_category_descriptor_section
           << "\",\"runtime_metadata_section_logical_property_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_property_descriptor_section
           << "\",\"runtime_metadata_section_logical_ivar_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_ivar_descriptor_section
           << "\",\"runtime_metadata_section_descriptor_symbol_prefix\":\""
           << runtime_metadata_section_abi.descriptor_symbol_prefix
           << "\",\"runtime_metadata_section_aggregate_symbol_prefix\":\""
           << runtime_metadata_section_abi.aggregate_symbol_prefix
           << "\",\"runtime_metadata_section_image_info_symbol\":\""
           << runtime_metadata_section_abi.image_info_symbol
           << "\",\"runtime_metadata_section_descriptor_linkage\":\""
           << runtime_metadata_section_abi.descriptor_linkage
           << "\",\"runtime_metadata_section_aggregate_linkage\":\""
           << runtime_metadata_section_abi.aggregate_linkage
           << "\",\"runtime_metadata_section_visibility\":\""
           << runtime_metadata_section_abi.metadata_visibility
           << "\",\"runtime_metadata_section_retention_root\":\""
           << runtime_metadata_section_abi.retention_root
           << "\",\"runtime_metadata_section_failure_reason\":\""
           << runtime_metadata_section_abi.failure_reason
           << "\""
           << ",\"runtime_metadata_section_publication_contract_id\":\""
           << runtime_metadata_section_publication.contract_id
           << "\",\"runtime_metadata_section_publication_abi_contract_id\":\""
           << runtime_metadata_section_publication.abi_contract_id
           << "\",\"runtime_metadata_section_publication_emitted\":"
           << (runtime_metadata_section_publication.publication_emitted ? "true"
                                                                  : "false")
           << ",\"runtime_metadata_section_publication_fail_closed\":"
           << (runtime_metadata_section_publication.fail_closed ? "true" : "false")
           << ",\"runtime_metadata_section_publication_uses_llvm_used\":"
           << (runtime_metadata_section_publication.uses_llvm_used ? "true"
                                                                : "false")
           << ",\"runtime_metadata_section_publication_image_info_emitted\":"
           << (runtime_metadata_section_publication.image_info_emitted ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_section_publication_class_descriptor_count\":"
           << runtime_metadata_section_publication.class_descriptor_count
           << ",\"runtime_metadata_section_publication_protocol_descriptor_count\":"
           << runtime_metadata_section_publication.protocol_descriptor_count
           << ",\"runtime_metadata_section_publication_category_descriptor_count\":"
           << runtime_metadata_section_publication.category_descriptor_count
           << ",\"runtime_metadata_section_publication_property_descriptor_count\":"
           << runtime_metadata_section_publication.property_descriptor_count
           << ",\"runtime_metadata_section_publication_ivar_descriptor_count\":"
           << runtime_metadata_section_publication.ivar_descriptor_count
           << ",\"runtime_metadata_section_publication_total_descriptor_count\":"
           << runtime_metadata_section_publication.total_descriptor_count
           << ",\"runtime_metadata_section_publication_total_retained_global_count\":"
           << runtime_metadata_section_publication.total_retained_global_count
           << ",\"runtime_metadata_section_publication_image_info_symbol\":\""
           << runtime_metadata_section_publication.image_info_symbol
           << "\",\"runtime_metadata_section_publication_class_aggregate_symbol\":\""
           << runtime_metadata_section_publication.class_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_protocol_aggregate_symbol\":\""
           << runtime_metadata_section_publication.protocol_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_category_aggregate_symbol\":\""
           << runtime_metadata_section_publication.category_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_property_aggregate_symbol\":\""
           << runtime_metadata_section_publication.property_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_ivar_aggregate_symbol\":\""
           << runtime_metadata_section_publication.ivar_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_failure_reason\":\""
           << runtime_metadata_section_publication.failure_reason
           << "\",\"runtime_metadata_object_inspection_contract_id\":\""
           << runtime_metadata_object_inspection.contract_id
           << "\",\"runtime_metadata_object_inspection_publication_contract_id\":\""
           << runtime_metadata_object_inspection.publication_contract_id
           << "\",\"runtime_metadata_object_inspection_matrix_published\":"
           << (runtime_metadata_object_inspection.matrix_published ? "true"
                                                                   : "false")
           << ",\"runtime_metadata_object_inspection_fail_closed\":"
           << (runtime_metadata_object_inspection.fail_closed ? "true"
                                                              : "false")
           << ",\"runtime_metadata_object_inspection_uses_llvm_readobj\":"
           << (runtime_metadata_object_inspection.uses_llvm_readobj ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_object_inspection_uses_llvm_objdump\":"
           << (runtime_metadata_object_inspection.uses_llvm_objdump ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_object_inspection_matrix_row_count\":"
           << runtime_metadata_object_inspection.matrix_row_count
           << ",\"runtime_metadata_object_inspection_fixture_path\":\""
           << runtime_metadata_object_inspection.fixture_path
           << "\",\"runtime_metadata_object_inspection_emit_prefix\":\""
           << runtime_metadata_object_inspection.emit_prefix
           << "\",\"runtime_metadata_object_inspection_object_relative_path\":\""
           << runtime_metadata_object_inspection.object_relative_path
           << "\",\"runtime_metadata_object_inspection_section_inventory_row_key\":\""
           << runtime_metadata_object_inspection.section_inventory_row_key
           << "\",\"runtime_metadata_object_inspection_section_inventory_command\":\""
           << runtime_metadata_object_inspection.section_inventory_command
           << "\",\"runtime_metadata_object_inspection_symbol_inventory_row_key\":\""
           << runtime_metadata_object_inspection.symbol_inventory_row_key
           << "\",\"runtime_metadata_object_inspection_symbol_inventory_command\":\""
           << runtime_metadata_object_inspection.symbol_inventory_command
           << "\",\"runtime_metadata_object_inspection_failure_reason\":\""
           << runtime_metadata_object_inspection.failure_reason
           << "\",\"executable_metadata_debug_projection_contract_id\":\""
           << EscapeJsonString(executable_metadata_debug_projection.contract_id)
           << "\",\"executable_metadata_debug_projection_typed_handoff_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection
                      .typed_lowering_handoff_contract_id)
           << "\",\"executable_metadata_debug_projection_source_graph_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.source_graph_contract_id)
           << "\",\"executable_metadata_debug_projection_named_metadata_name\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.named_metadata_name)
           << "\",\"executable_metadata_debug_projection_manifest_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.manifest_surface_path)
           << "\",\"executable_metadata_debug_projection_typed_handoff_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.typed_handoff_surface_path)
           << "\",\"executable_metadata_debug_projection_source_graph_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.source_graph_surface_path)
           << "\",\"executable_metadata_debug_projection_matrix_published\":"
           << (executable_metadata_debug_projection.matrix_published ? "true"
                                                                    : "false")
           << ",\"executable_metadata_debug_projection_fail_closed\":"
           << (executable_metadata_debug_projection.fail_closed ? "true"
                                                                : "false")
           << ",\"executable_metadata_debug_projection_manifest_debug_surface_published\":"
           << (executable_metadata_debug_projection
                       .manifest_debug_surface_published
                   ? "true"
                   : "false")
           << ",\"executable_metadata_debug_projection_ir_named_metadata_published\":"
           << (executable_metadata_debug_projection.ir_named_metadata_published
                   ? "true"
                   : "false")
           << ",\"executable_metadata_debug_projection_replay_anchor_deterministic\":"
           << (executable_metadata_debug_projection.replay_anchor_deterministic
                   ? "true"
                   : "false")
           << ",\"executable_metadata_debug_projection_active_typed_handoff_ready\":"
           << (executable_metadata_debug_projection.active_typed_handoff_ready
                   ? "true"
                   : "false")
           << ",\"executable_metadata_debug_projection_matrix_row_count\":"
           << executable_metadata_debug_projection.matrix_row_count
           << ",\"executable_metadata_debug_projection_replay_key\":\""
           << EscapeJsonString(executable_metadata_debug_projection.replay_key)
           << "\",\"executable_metadata_debug_projection_active_typed_handoff_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection
                      .active_typed_handoff_replay_key)
           << "\",\"executable_metadata_debug_projection_failure_reason\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.failure_reason)
           << "\",\"executable_metadata_runtime_ingest_packaging_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .contract_id)
           << "\",\"executable_metadata_runtime_ingest_packaging_typed_handoff_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .typed_lowering_handoff_contract_id)
           << "\",\"executable_metadata_runtime_ingest_packaging_debug_projection_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .debug_projection_contract_id)
           << "\",\"executable_metadata_runtime_ingest_packaging_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .packaging_surface_path)
           << "\",\"executable_metadata_runtime_ingest_packaging_typed_handoff_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .typed_handoff_surface_path)
           << "\",\"executable_metadata_runtime_ingest_packaging_debug_projection_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .debug_projection_surface_path)
           << "\",\"executable_metadata_runtime_ingest_packaging_payload_model\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .packaging_payload_model)
           << "\",\"executable_metadata_runtime_ingest_packaging_transport_artifact_relative_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .transport_artifact_relative_path)
           << "\",\"executable_metadata_runtime_ingest_packaging_boundary_frozen\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .boundary_frozen
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_fail_closed\":"
           << (executable_metadata_runtime_ingest_packaging_contract.fail_closed
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_typed_handoff_ready\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .typed_lowering_handoff_ready
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_debug_projection_ready\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .debug_projection_ready
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_manifest_transport_frozen\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .manifest_transport_frozen
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_runtime_section_emission_not_yet_landed\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .runtime_section_emission_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_startup_registration_not_yet_landed\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .startup_registration_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_runtime_loader_registration_not_yet_landed\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .runtime_loader_registration_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_explicit_non_goals_published\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .explicit_non_goals_published
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_ready_for_packaging_implementation\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .ready_for_packaging_implementation
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_typed_handoff_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .typed_lowering_handoff_replay_key)
           << "\",\"executable_metadata_runtime_ingest_packaging_debug_projection_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .debug_projection_replay_key)
           << "\",\"executable_metadata_runtime_ingest_packaging_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .replay_key)
           << "\",\"executable_metadata_runtime_ingest_packaging_failure_reason\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .failure_reason)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .contract_id)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_packaging_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .packaging_contract_id)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .binary_boundary_surface_path)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_payload_model\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .payload_model)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_envelope_format\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .envelope_format)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_artifact_relative_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .artifact_relative_path)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_artifact_suffix\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .artifact_suffix)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_magic\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .binary_magic)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_chunk_count\":"
           << executable_metadata_runtime_ingest_binary_boundary.chunk_count
           << ",\"executable_metadata_runtime_ingest_binary_boundary_fail_closed\":"
           << (executable_metadata_runtime_ingest_binary_boundary.fail_closed
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_binary_boundary_binary_payload_present\":"
           << (executable_metadata_runtime_ingest_binary_boundary
                       .binary_payload_present
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_binary_boundary_binary_envelope_deterministic\":"
           << (executable_metadata_runtime_ingest_binary_boundary
                       .binary_envelope_deterministic
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_binary_boundary_ready_for_section_emission_handoff\":"
           << (executable_metadata_runtime_ingest_binary_boundary
                       .ready_for_section_emission_handoff
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_binary_boundary_payload_bytes\":"
           << executable_metadata_runtime_ingest_binary_boundary.payload_bytes
           << ",\"executable_metadata_runtime_ingest_binary_boundary_packaging_contract_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .packaging_contract_replay_key)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_typed_handoff_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .typed_lowering_handoff_replay_key)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_debug_projection_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .debug_projection_replay_key)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .replay_key)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_failure_reason\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .failure_reason)
           << "\",\"runtime_support_library_contract_id\":\""
           << runtime_support_library.contract_id
           << "\",\"runtime_support_library_metadata_publication_contract_id\":\""
           << runtime_support_library.metadata_publication_contract_id
           << "\",\"runtime_support_library_boundary_frozen\":"
           << (runtime_support_library.boundary_frozen ? "true" : "false")
           << ",\"runtime_support_library_fail_closed\":"
           << (runtime_support_library.fail_closed ? "true" : "false")
           << ",\"runtime_support_library_target_name_frozen\":"
           << (runtime_support_library.target_name_frozen ? "true" : "false")
           << ",\"runtime_support_library_exported_entrypoints_frozen\":"
           << (runtime_support_library.exported_entrypoints_frozen ? "true"
                                                                   : "false")
           << ",\"runtime_support_library_ownership_boundaries_frozen\":"
           << (runtime_support_library.ownership_boundaries_frozen ? "true"
                                                                   : "false")
           << ",\"runtime_support_library_build_constraints_frozen\":"
           << (runtime_support_library.build_constraints_frozen ? "true"
                                                                : "false")
           << ",\"runtime_support_library_shim_remains_test_only\":"
           << (runtime_support_library.shim_remains_test_only ? "true"
                                                              : "false")
           << ",\"runtime_support_library_native_library_present\":"
           << (runtime_support_library.native_runtime_library_present ? "true"
                                                                      : "false")
           << ",\"runtime_support_library_driver_link_wiring_pending\":"
           << (runtime_support_library.driver_link_wiring_pending ? "true"
                                                                  : "false")
           << ",\"runtime_support_library_ready_for_skeleton\":"
           << (runtime_support_library.ready_for_runtime_library_skeleton
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_target_name\":\""
           << runtime_support_library.cmake_target_name
           << "\",\"runtime_support_library_public_header_path\":\""
           << runtime_support_library.public_header_path
           << "\",\"runtime_support_library_source_root\":\""
           << runtime_support_library.source_root
           << "\",\"runtime_support_library_library_kind\":\""
           << runtime_support_library.library_kind
           << "\",\"runtime_support_library_archive_basename\":\""
           << runtime_support_library.archive_basename
           << "\",\"runtime_support_library_register_image_symbol\":\""
           << runtime_support_library.register_image_symbol
           << "\",\"runtime_support_library_lookup_selector_symbol\":\""
           << runtime_support_library.lookup_selector_symbol
           << "\",\"runtime_support_library_dispatch_i32_symbol\":\""
           << runtime_support_library.dispatch_i32_symbol
           << "\",\"runtime_support_library_reset_for_testing_symbol\":\""
           << runtime_support_library.reset_for_testing_symbol
           << "\",\"runtime_support_library_driver_link_mode\":\""
           << runtime_support_library.driver_link_mode
           << "\",\"runtime_support_library_compiler_ownership_boundary\":\""
           << runtime_support_library.compiler_ownership_boundary
           << "\",\"runtime_support_library_runtime_ownership_boundary\":\""
           << runtime_support_library.runtime_ownership_boundary
           << "\",\"runtime_support_library_failure_reason\":\""
           << runtime_support_library.failure_reason
           << "\",\"runtime_support_library_core_feature_contract_id\":\""
           << runtime_support_library_core_feature.contract_id
           << "\",\"runtime_support_library_core_feature_support_library_contract_id\":\""
           << runtime_support_library_core_feature.support_library_contract_id
           << "\",\"runtime_support_library_core_feature_metadata_publication_contract_id\":\""
           << runtime_support_library_core_feature.metadata_publication_contract_id
           << "\",\"runtime_support_library_core_feature_fail_closed\":"
           << (runtime_support_library_core_feature.fail_closed ? "true"
                                                                : "false")
           << ",\"runtime_support_library_core_feature_sources_present\":"
           << (runtime_support_library_core_feature
                           .native_runtime_library_sources_present
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_header_present\":"
           << (runtime_support_library_core_feature
                           .native_runtime_library_header_present
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_archive_build_enabled\":"
           << (runtime_support_library_core_feature
                           .native_runtime_library_archive_build_enabled
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_entrypoints_implemented\":"
           << (runtime_support_library_core_feature
                           .native_runtime_library_entrypoints_implemented
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_selector_lookup_stateful\":"
           << (runtime_support_library_core_feature.selector_lookup_stateful
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_dispatch_formula_matches_test_shim\":"
           << (runtime_support_library_core_feature
                           .deterministic_dispatch_formula_matches_test_shim
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_reset_for_testing_supported\":"
           << (runtime_support_library_core_feature.reset_for_testing_supported
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_shim_remains_test_only\":"
           << (runtime_support_library_core_feature.shim_remains_test_only
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_driver_link_wiring_pending\":"
           << (runtime_support_library_core_feature.driver_link_wiring_pending
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_ready_for_driver_link_wiring\":"
           << (runtime_support_library_core_feature.ready_for_driver_link_wiring
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_target_name\":\""
           << runtime_support_library_core_feature.cmake_target_name
           << "\",\"runtime_support_library_core_feature_public_header_path\":\""
           << runtime_support_library_core_feature.public_header_path
           << "\",\"runtime_support_library_core_feature_source_root\":\""
           << runtime_support_library_core_feature.source_root
           << "\",\"runtime_support_library_core_feature_implementation_source_path\":\""
           << runtime_support_library_core_feature.implementation_source_path
           << "\",\"runtime_support_library_core_feature_library_kind\":\""
           << runtime_support_library_core_feature.library_kind
           << "\",\"runtime_support_library_core_feature_archive_basename\":\""
           << runtime_support_library_core_feature.archive_basename
           << "\",\"runtime_support_library_core_feature_archive_relative_path\":\""
           << runtime_support_library_core_feature.archive_relative_path
           << "\",\"runtime_support_library_core_feature_probe_source_path\":\""
           << runtime_support_library_core_feature.probe_source_path
           << "\",\"runtime_support_library_core_feature_register_image_symbol\":\""
           << runtime_support_library_core_feature.register_image_symbol
           << "\",\"runtime_support_library_core_feature_lookup_selector_symbol\":\""
           << runtime_support_library_core_feature.lookup_selector_symbol
           << "\",\"runtime_support_library_core_feature_dispatch_i32_symbol\":\""
           << runtime_support_library_core_feature.dispatch_i32_symbol
           << "\",\"runtime_support_library_core_feature_reset_for_testing_symbol\":\""
           << runtime_support_library_core_feature.reset_for_testing_symbol
           << "\",\"runtime_support_library_core_feature_driver_link_mode\":\""
           << runtime_support_library_core_feature.driver_link_mode
           << "\",\"runtime_support_library_link_wiring_contract_id\":\""
           << runtime_support_library_link_wiring.contract_id
           << "\",\"runtime_support_library_link_wiring_core_feature_contract_id\":\""
           << runtime_support_library_link_wiring
                  .support_library_core_feature_contract_id
           << "\",\"runtime_support_library_link_wiring_fail_closed\":"
           << (runtime_support_library_link_wiring.fail_closed ? "true"
                                                               : "false")
           << ",\"runtime_support_library_link_wiring_archive_available\":"
           << (runtime_support_library_link_wiring
                       .runtime_library_archive_available
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_compatibility_dispatch_alias_exported\":"
           << (runtime_support_library_link_wiring
                       .compatibility_dispatch_alias_exported
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_driver_emits_runtime_link_contract\":"
           << (runtime_support_library_link_wiring
                       .driver_emits_runtime_link_contract
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_execution_smoke_consumes_runtime_library\":"
           << (runtime_support_library_link_wiring
                       .execution_smoke_consumes_runtime_library
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_shim_remains_test_only\":"
           << (runtime_support_library_link_wiring.shim_remains_test_only
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_ready_for_runtime_library_consumption\":"
           << (runtime_support_library_link_wiring
                       .ready_for_runtime_library_consumption
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_archive_relative_path\":\""
           << runtime_support_library_link_wiring.archive_relative_path
           << "\",\"runtime_support_library_link_wiring_compatibility_dispatch_symbol\":\""
           << runtime_support_library_link_wiring.compatibility_dispatch_symbol
           << "\",\"runtime_support_library_link_wiring_runtime_dispatch_symbol\":\""
           << runtime_support_library_link_wiring.runtime_dispatch_symbol
           << "\",\"runtime_support_library_link_wiring_execution_smoke_script_path\":\""
           << runtime_support_library_link_wiring.execution_smoke_script_path
           << "\",\"runtime_support_library_link_wiring_driver_link_mode\":\""
           << runtime_support_library_link_wiring.driver_link_mode
           << "\",\"runtime_support_library_link_wiring_failure_reason\":\""
           << runtime_support_library_link_wiring.failure_reason
           << "\",\"runtime_support_library_core_feature_failure_reason\":\""
           << runtime_support_library_core_feature.failure_reason
           << "\",\"runtime_translation_unit_registration_contract_id\":\""
           << runtime_translation_unit_registration_contract.contract_id
           << "\",\"runtime_translation_unit_registration_binary_boundary_contract_id\":\""
           << runtime_translation_unit_registration_contract
                  .binary_boundary_contract_id
           << "\",\"runtime_translation_unit_registration_archive_static_link_contract_id\":\""
           << runtime_translation_unit_registration_contract
                  .archive_static_link_contract_id
           << "\",\"runtime_translation_unit_registration_object_emission_closeout_contract_id\":\""
           << runtime_translation_unit_registration_contract
                  .object_emission_closeout_contract_id
           << "\",\"runtime_translation_unit_registration_runtime_support_library_link_wiring_contract_id\":\""
           << runtime_translation_unit_registration_contract
                  .runtime_support_library_link_wiring_contract_id
           << "\",\"runtime_translation_unit_registration_payload_model\":\""
           << runtime_translation_unit_registration_contract
                  .registration_payload_model
           << "\",\"runtime_translation_unit_registration_runtime_owned_payload_artifact_count\":"
           << runtime_translation_unit_registration_contract
                  .runtime_owned_payload_artifact_count
           << ",\"runtime_translation_unit_registration_payload_artifact_relative_path\":\""
           << runtime_translation_unit_registration_contract
                  .runtime_owned_payload_artifacts[0]
           << "\",\"runtime_translation_unit_registration_linker_response_artifact_relative_path\":\""
           << runtime_translation_unit_registration_contract
                  .runtime_owned_payload_artifacts[1]
           << "\",\"runtime_translation_unit_registration_discovery_artifact_relative_path\":\""
           << runtime_translation_unit_registration_contract
                  .runtime_owned_payload_artifacts[2]
           << "\",\"runtime_translation_unit_registration_constructor_root_symbol\":\""
           << runtime_translation_unit_registration_contract
                  .constructor_root_symbol
           << "\",\"runtime_translation_unit_registration_constructor_root_ownership_model\":\""
           << runtime_translation_unit_registration_contract
                  .constructor_root_ownership_model
           << "\",\"runtime_translation_unit_registration_constructor_emission_mode\":\""
           << runtime_translation_unit_registration_contract
                  .constructor_emission_mode
           << "\",\"runtime_translation_unit_registration_constructor_priority_policy\":\""
           << runtime_translation_unit_registration_contract
                  .constructor_priority_policy
           << "\",\"runtime_translation_unit_registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_contract
                  .registration_entrypoint_symbol
           << "\",\"runtime_translation_unit_registration_translation_unit_identity_model\":\""
           << runtime_translation_unit_registration_contract
                  .translation_unit_identity_model
           << "\",\"runtime_translation_unit_registration_boundary_frozen\":"
           << (runtime_translation_unit_registration_contract.boundary_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_fail_closed\":"
           << (runtime_translation_unit_registration_contract.fail_closed
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_binary_boundary_ready\":"
           << (runtime_translation_unit_registration_contract.binary_boundary_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_archive_static_link_surface_ready\":"
           << (runtime_translation_unit_registration_contract
                       .archive_static_link_surface_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_object_emission_closeout_surface_ready\":"
           << (runtime_translation_unit_registration_contract
                       .object_emission_closeout_surface_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_runtime_support_library_link_wiring_ready\":"
           << (runtime_translation_unit_registration_contract
                       .runtime_support_library_link_wiring_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_runtime_owned_payload_inventory_published\":"
           << (runtime_translation_unit_registration_contract
                       .runtime_owned_payload_inventory_published
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_constructor_root_reserved_not_emitted\":"
           << (runtime_translation_unit_registration_contract
                       .constructor_root_reserved_not_emitted
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_startup_registration_not_yet_landed\":"
           << (runtime_translation_unit_registration_contract
                       .startup_registration_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_runtime_bootstrap_not_yet_landed\":"
           << (runtime_translation_unit_registration_contract
                       .runtime_bootstrap_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_explicit_non_goals_published\":"
           << (runtime_translation_unit_registration_contract
                       .explicit_non_goals_published
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_ready_for_manifest_implementation\":"
           << (runtime_translation_unit_registration_contract
                       .ready_for_registration_manifest_implementation
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_binary_boundary_replay_key\":\""
           << EscapeJsonString(
                  runtime_translation_unit_registration_contract
                      .binary_boundary_replay_key)
           << "\",\"runtime_translation_unit_registration_replay_key\":\""
           << EscapeJsonString(
                  runtime_translation_unit_registration_contract.replay_key)
           << "\",\"runtime_translation_unit_registration_failure_reason\":\""
           << EscapeJsonString(
                  runtime_translation_unit_registration_contract.failure_reason)
           << "\""
           << ",\"runtime_translation_unit_registration_manifest_contract_id\":\""
           << runtime_translation_unit_registration_manifest.contract_id
           << "\",\"runtime_translation_unit_registration_manifest_payload_model\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_payload_model
           << "\",\"runtime_translation_unit_registration_manifest_artifact_relative_path\":\""
           << runtime_translation_unit_registration_manifest
