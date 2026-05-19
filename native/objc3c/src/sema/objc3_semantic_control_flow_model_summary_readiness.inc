static void FinalizeControlFlowControlFlowSemanticModelReadiness(
    Objc3ControlFlowControlFlowSemanticModelSummary &summary) {
  summary.source_dependency_required = true;
  summary.guard_refinement_semantics_landed =
      summary.guard_refinement_sites ==
      summary.guard_binding_clause_semantic_sites;
  summary.guard_exit_enforcement_landed =
      summary.guard_exit_enforcement_sites ==
      summary.guard_binding_semantic_sites +
          summary.guard_condition_statement_sites;
  summary.match_binding_scope_semantics_landed =
      summary.match_binding_scope_sites <=
      summary.match_statement_semantic_sites;
  summary.match_result_case_scope_semantics_landed =
      summary.match_result_case_scope_sites <=
      summary.match_statement_semantic_sites;
  summary.match_exhaustiveness_semantics_landed =
      summary.match_statement_semantic_sites ==
      summary.match_exhaustive_statement_sites +
          summary.match_non_exhaustive_diagnostic_sites;
  summary.match_exhaustiveness_deferred =
      summary.match_exhaustiveness_deferred_sites != 0;
  summary.defer_cleanup_order_semantics_landed = true;
  summary.defer_nonlocal_exit_semantics_landed = true;
  summary.defer_cleanup_order_deferred = false;
  summary.defer_nonlocal_exit_deferred = false;
  summary.non_local_exit_restrictions_landed =
      summary.break_restriction_diagnostic_sites <=
          summary.break_statement_sites &&
      summary.continue_restriction_diagnostic_sites <=
          summary.continue_statement_sites;
  summary.deterministic =
      summary.guard_binding_semantic_sites <=
          summary.guard_binding_clause_semantic_sites &&
      summary.guard_condition_statement_sites <=
          summary.guard_exit_enforcement_sites &&
      summary.guard_condition_clause_semantic_sites >=
          summary.guard_condition_statement_sites &&
      summary.match_default_pattern_sites <=
          summary.match_statement_semantic_sites &&
      summary.match_wildcard_pattern_sites <=
          summary.match_statement_semantic_sites &&
      summary.match_literal_pattern_sites <=
          summary.match_statement_semantic_sites &&
      summary.match_binding_scope_sites <=
          summary.match_statement_semantic_sites &&
      summary.match_result_case_scope_sites <=
          summary.match_statement_semantic_sites &&
      summary.match_exhaustive_statement_sites <=
          summary.match_statement_semantic_sites &&
      summary.match_non_exhaustive_diagnostic_sites <=
          summary.match_statement_semantic_sites &&
      summary.match_exhaustiveness_deferred_sites == 0 &&
      summary.match_exhaustiveness_semantics_landed &&
      !summary.match_exhaustiveness_deferred &&
      summary.defer_scope_cleanup_order_sites <=
          summary.defer_statement_semantic_sites &&
      !summary.defer_cleanup_order_deferred &&
      !summary.defer_nonlocal_exit_deferred &&
      summary.defer_cleanup_order_semantics_landed &&
      summary.defer_nonlocal_exit_semantics_landed &&
      summary.non_local_exit_restrictions_landed &&
      summary.guard_refinement_semantics_landed &&
      summary.guard_exit_enforcement_landed &&
      summary.match_binding_scope_semantics_landed &&
      summary.match_result_case_scope_semantics_landed;
  summary.ready_for_lowering_and_runtime =
      summary.source_dependency_required &&
      summary.defer_cleanup_order_semantics_landed &&
      summary.defer_nonlocal_exit_semantics_landed &&
      !summary.defer_cleanup_order_deferred &&
      !summary.defer_nonlocal_exit_deferred &&
      summary.deterministic;
}
