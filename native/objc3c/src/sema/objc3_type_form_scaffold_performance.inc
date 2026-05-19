  summary.performance_quality_required_guardrail_count = 9u;
  summary.performance_quality_passed_guardrail_count = 0u;
  summary.performance_quality_passed_guardrail_count +=
      summary.conformance_matrix_consistent ? 1u : 0u;
  summary.performance_quality_passed_guardrail_count +=
      summary.conformance_matrix_ready ? 1u : 0u;
  summary.performance_quality_passed_guardrail_count +=
      !summary.conformance_matrix_key.empty() ? 1u : 0u;
  summary.performance_quality_passed_guardrail_count +=
      summary.conformance_corpus_case_count > 0u ? 1u : 0u;
  summary.performance_quality_passed_guardrail_count +=
      summary.conformance_corpus_passed_case_count ==
              summary.conformance_corpus_case_count
          ? 1u
          : 0u;
  summary.performance_quality_passed_guardrail_count +=
      summary.conformance_corpus_failed_case_count == 0u ? 1u : 0u;
  summary.performance_quality_passed_guardrail_count +=
      summary.conformance_corpus_consistent ? 1u : 0u;
  summary.performance_quality_passed_guardrail_count +=
      summary.conformance_corpus_ready ? 1u : 0u;
  summary.performance_quality_passed_guardrail_count +=
      !summary.conformance_corpus_key.empty() ? 1u : 0u;
  summary.performance_quality_failed_guardrail_count =
      summary.performance_quality_required_guardrail_count -
      summary.performance_quality_passed_guardrail_count;
  summary.performance_quality_guardrails_consistent =
      summary.performance_quality_required_guardrail_count > 0u &&
      summary.performance_quality_failed_guardrail_count == 0u;
  summary.performance_quality_guardrails_key =
      std::string("type-form-performance-quality-guardrails;required=") +
      std::to_string(summary.performance_quality_required_guardrail_count) +
      ";passed=" +
      std::to_string(summary.performance_quality_passed_guardrail_count) +
      ";failed=" +
      std::to_string(summary.performance_quality_failed_guardrail_count);
  summary.performance_quality_guardrails_ready =
      summary.performance_quality_guardrails_consistent &&
      !summary.performance_quality_guardrails_key.empty();
  summary.deterministic = summary.canonical_reference_form_count > 0 &&
                          summary.canonical_message_scalar_form_count > 0 &&
                          summary.canonical_bridge_top_form_count > 0 &&
                          summary.canonical_reference_forms_unique &&
                          summary.canonical_message_scalar_forms_unique &&
                          summary.canonical_bridge_top_forms_unique &&
                          summary.canonical_message_scalars_disjoint_from_reference &&
                          summary.canonical_bridge_top_subset_of_reference &&
                          summary.canonical_bridge_top_excludes_sel &&
                          summary.canonical_reference_includes_sel &&
                          summary.canonical_message_scalars_include_i32 &&
                          summary.canonical_message_scalars_include_bool &&
                          summary.canonical_forms_exclude_unknown &&
                          summary.canonical_bridge_top_matches_reference_without_sel &&
                          summary.diagnostics_hardening_consistent &&
                          summary.diagnostics_hardening_ready &&
                          !summary.diagnostics_hardening_key.empty() &&
                          summary.recovery_determinism_consistent &&
                          summary.recovery_determinism_ready &&
                          !summary.recovery_determinism_key.empty() &&
                          summary.conformance_matrix_consistent &&
                          summary.conformance_matrix_ready &&
                          !summary.conformance_matrix_key.empty() &&
                          summary.conformance_corpus_case_count > 0u &&
                          summary.conformance_corpus_passed_case_count ==
                              summary.conformance_corpus_case_count &&
                          summary.conformance_corpus_failed_case_count == 0u &&
                          summary.conformance_corpus_consistent &&
                          summary.conformance_corpus_ready &&
                          !summary.conformance_corpus_key.empty() &&
                          summary.performance_quality_required_guardrail_count >
                              0u &&
                          summary.performance_quality_passed_guardrail_count ==
                              summary
                                  .performance_quality_required_guardrail_count &&
                          summary.performance_quality_failed_guardrail_count ==
                              0u &&
                          summary.performance_quality_guardrails_consistent &&
                          summary.performance_quality_guardrails_ready &&
                          !summary.performance_quality_guardrails_key.empty();
