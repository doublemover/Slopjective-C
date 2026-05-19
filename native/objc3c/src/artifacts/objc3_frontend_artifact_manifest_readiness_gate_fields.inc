  manifest << "\"ready_for_lowering\": "
           << (readiness_surface.ready_for_lowering ? "true" : "false")
           << ",\"parser_contract_snapshot_present\": "
           << (readiness_surface.parser_contract_snapshot_present ? "true" : "false")
           << ",\"long_tail_grammar_core_feature_consistent\": "
           << (readiness_surface.long_tail_grammar_core_feature_consistent ? "true" : "false")
           << ",\"long_tail_grammar_handoff_key_deterministic\": "
           << (readiness_surface.long_tail_grammar_handoff_key_deterministic ? "true"
                                                                              : "false")
           << ",\"long_tail_grammar_expansion_accounting_consistent\": "
           << (readiness_surface.long_tail_grammar_expansion_accounting_consistent ? "true"
                                                                                    : "false")
           << ",\"long_tail_grammar_replay_keys_ready\": "
           << (readiness_surface.long_tail_grammar_replay_keys_ready ? "true" : "false")
           << ",\"long_tail_grammar_expansion_ready\": "
           << (readiness_surface.long_tail_grammar_expansion_ready ? "true" : "false")
           << ",\"long_tail_grammar_compatibility_handoff_ready\": "
           << (readiness_surface.long_tail_grammar_compatibility_handoff_ready ? "true"
                                                                                : "false")
           << ",\"long_tail_grammar_edge_case_compatibility_consistent\": "
           << (readiness_surface.long_tail_grammar_edge_case_compatibility_consistent ? "true"
                                                                                       : "false")
           << ",\"long_tail_grammar_edge_case_compatibility_ready\": "
           << (readiness_surface.long_tail_grammar_edge_case_compatibility_ready ? "true"
                                                                                  : "false")
           << ",\"long_tail_grammar_edge_case_expansion_consistent\": "
           << (readiness_surface.long_tail_grammar_edge_case_expansion_consistent ? "true"
                                                                                   : "false")
           << ",\"long_tail_grammar_edge_case_robustness_ready\": "
           << (readiness_surface.long_tail_grammar_edge_case_robustness_ready ? "true" : "false")
           << ",\"long_tail_grammar_diagnostics_hardening_consistent\": "
           << (readiness_surface.long_tail_grammar_diagnostics_hardening_consistent ? "true"
                                                                                     : "false")
           << ",\"long_tail_grammar_diagnostics_hardening_ready\": "
           << (readiness_surface.long_tail_grammar_diagnostics_hardening_ready ? "true"
                                                                                : "false")
           << ",\"long_tail_grammar_recovery_determinism_consistent\": "
           << (readiness_surface.long_tail_grammar_recovery_determinism_consistent ? "true"
                                                                                    : "false")
           << ",\"long_tail_grammar_recovery_determinism_ready\": "
           << (readiness_surface.long_tail_grammar_recovery_determinism_ready ? "true"
                                                                               : "false")
           << ",\"long_tail_grammar_conformance_matrix_consistent\": "
           << (readiness_surface.long_tail_grammar_conformance_matrix_consistent ? "true"
                                                                                  : "false")
           << ",\"long_tail_grammar_conformance_matrix_ready\": "
           << (readiness_surface.long_tail_grammar_conformance_matrix_ready ? "true"
                                                                             : "false")
           << ",\"long_tail_grammar_integration_closeout_consistent\": "
           << (readiness_surface.long_tail_grammar_integration_closeout_consistent ? "true"
                                                                                    : "false")
           << ",\"long_tail_grammar_gate_signoff_ready\": "
           << (readiness_surface.long_tail_grammar_gate_signoff_ready ? "true" : "false")
           << ",\"parse_artifact_handoff_consistent\": "
           << (readiness_surface.parse_artifact_handoff_consistent ? "true" : "false")
           << ",\"parse_artifact_handoff_deterministic\": "
           << (readiness_surface.parse_artifact_handoff_deterministic ? "true" : "false")
           << ",\"parser_token_count_budget_consistent\": "
           << (readiness_surface.parser_token_count_budget_consistent ? "true" : "false")
           << ",\"parse_artifact_layout_fingerprint_consistent\": "
           << (readiness_surface.parse_artifact_layout_fingerprint_consistent ? "true" : "false")
           << ",\"parse_artifact_fingerprint_consistent\": "
           << (readiness_surface.parse_artifact_fingerprint_consistent ? "true" : "false")
           << ",\"compatibility_handoff_consistent\": "
           << (readiness_surface.compatibility_handoff_consistent ? "true" : "false")
           << ",\"parser_diagnostic_surface_consistent\": "
           << (readiness_surface.parser_diagnostic_surface_consistent ? "true" : "false")
           << ",\"parser_diagnostic_code_surface_deterministic\": "
           << (readiness_surface.parser_diagnostic_code_surface_deterministic ? "true"
                                                                              : "false")
           << ",\"language_version_pragma_coordinate_order_consistent\": "
           << (readiness_surface.language_version_pragma_coordinate_order_consistent ? "true"
                                                                                     : "false")
           << ",\"parse_artifact_replay_key_deterministic\": "
           << (readiness_surface.parse_artifact_replay_key_deterministic ? "true" : "false")
           << ",\"parse_artifact_diagnostics_hardening_consistent\": "
           << (readiness_surface.parse_artifact_diagnostics_hardening_consistent ? "true"
                                                                                  : "false")
           << ",\"parse_artifact_edge_case_robustness_consistent\": "
           << (readiness_surface.parse_artifact_edge_case_robustness_consistent ? "true"
                                                                                 : "false")
           << ",\"parse_recovery_determinism_hardening_consistent\": "
           << (readiness_surface.parse_recovery_determinism_hardening_consistent ? "true"
                                                                                 : "false")
           << ",\"parser_diagnostic_grammar_hooks_recovery_determinism_consistent\": "
           << (readiness_surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_recovery_determinism_ready\": "
           << (readiness_surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_matrix_consistent\": "
           << (readiness_surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_matrix_ready\": "
           << (readiness_surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_corpus_consistent\": "
           << (readiness_surface.parser_diagnostic_grammar_hooks_conformance_corpus_consistent
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_corpus_ready\": "
           << (readiness_surface.parser_diagnostic_grammar_hooks_conformance_corpus_ready
                   ? "true"
                   : "false")
           << ",\"parse_lowering_conformance_matrix_consistent\": "
           << (readiness_surface.parse_lowering_conformance_matrix_consistent ? "true"
                                                                              : "false")
           << ",\"parse_lowering_conformance_corpus_consistent\": "
           << (readiness_surface.parse_lowering_conformance_corpus_consistent ? "true"
                                                                              : "false")
           << ",\"parse_lowering_performance_quality_guardrails_consistent\": "
           << (readiness_surface.parse_lowering_performance_quality_guardrails_consistent
                   ? "true"
                   : "false");
