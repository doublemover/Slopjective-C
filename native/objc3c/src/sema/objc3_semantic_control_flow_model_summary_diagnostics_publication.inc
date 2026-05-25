static void PublishControlFlowControlFlowSemanticModelReplayKey(
    Objc3ControlFlowControlFlowSemanticModelSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source-dependency=" << summary.frontend_dependency_contract_id
      << ";guard-sites=" << summary.guard_binding_semantic_sites
      << ";guard-clauses=" << summary.guard_binding_clause_semantic_sites
      << ";guard-condition-statements="
      << summary.guard_condition_statement_sites
      << ";guard-condition-clauses="
      << summary.guard_condition_clause_semantic_sites
      << ";guard-exit=" << summary.guard_exit_enforcement_sites
      << ";guard-refinement=" << summary.guard_refinement_sites
      << ";match-sites=" << summary.match_statement_semantic_sites
      << ";match-defaults=" << summary.match_default_pattern_sites
      << ";match-wildcards=" << summary.match_wildcard_pattern_sites
      << ";match-literals=" << summary.match_literal_pattern_sites
      << ";match-bindings=" << summary.match_binding_scope_sites
      << ";match-result-bindings="
      << summary.match_result_case_scope_sites
      << ";match-guards=" << summary.match_guard_condition_sites
      << ";match-exhaustive=" << summary.match_exhaustive_statement_sites
      << ";match-bool-exhaustive=" << summary.match_bool_exhaustive_sites
      << ";match-result-exhaustive="
      << summary.match_result_case_exhaustive_sites
      << ";match-nonexhaustive-diagnostics="
      << summary.match_non_exhaustive_diagnostic_sites
      << ";match-exhaustiveness-deferred="
      << summary.match_exhaustiveness_deferred_sites
      << ";match-expression-sites="
      << summary.match_expression_semantic_sites
      << ";match-expression-result-types="
      << summary.match_expression_result_type_sites
      << ";match-expression-guards="
      << summary.match_expression_guard_condition_sites
      << ";match-expression-bindings="
      << summary.match_expression_binding_scope_sites
      << ";match-expression-result-bindings="
      << summary.match_expression_result_case_scope_sites
      << ";match-expression-exhaustive="
      << summary.match_expression_exhaustive_sites
      << ";match-expression-nonexhaustive-diagnostics="
      << summary.match_expression_non_exhaustive_diagnostic_sites
      << ";match-expression-lowering-eligible="
      << summary.match_expression_lowering_eligible_sites
      << ";match-expression-guard-effect-fail-closed="
      << summary.match_expression_guard_effect_fail_closed_sites
      << ";match-expression-result-type-mismatch="
      << summary.match_expression_result_type_mismatch_sites
      << ";defer-sites=" << summary.defer_statement_semantic_sites
      << ";defer-cleanup-order-sites="
      << summary.defer_scope_cleanup_order_sites
      << ";defer-nonlocal-exit-diagnostics="
      << summary.defer_nonlocal_exit_diagnostic_sites
      << ";break-sites=" << summary.break_statement_sites
      << ";continue-sites=" << summary.continue_statement_sites
      << ";break-diagnostics=" << summary.break_restriction_diagnostic_sites
      << ";continue-diagnostics="
      << summary.continue_restriction_diagnostic_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
}
