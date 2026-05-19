static void FinalizeErrorHandlingTryDoCatchReplayKey(
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";try=" << summary.try_expression_sites
      << ";throw=" << summary.throw_statement_sites
      << ";do-catch=" << summary.do_catch_sites
      << ";violations=" << summary.contract_violation_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
}

static void FinalizeErrorHandlingTryDoCatchReadiness(
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary) {
  summary.try_surface_landed =
      summary.try_expression_sites ==
          summary.try_propagating_sites + summary.try_optional_sites +
              summary.try_forced_sites &&
      summary.throwing_callable_try_sites <= summary.try_expression_sites &&
      summary.bridged_callable_try_sites <= summary.try_expression_sites;
  summary.throw_surface_landed =
      summary.rethrow_sites <= summary.throw_statement_sites;
  summary.do_catch_surface_landed =
      summary.catch_binding_sites <= summary.catch_clause_sites &&
      summary.catch_all_sites <= summary.catch_clause_sites;
  summary.throwing_context_legality_enforced = true;
  summary.deterministic =
      summary.try_surface_landed && summary.throw_surface_landed &&
      summary.do_catch_surface_landed &&
      summary.contract_violation_sites <=
          summary.try_expression_sites + summary.throw_statement_sites +
              summary.do_catch_sites + summary.catch_clause_sites;
  summary.ready_for_lowering_and_runtime = false;
}

static void FinalizeErrorHandlingTryDoCatchSummary(
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary) {
  FinalizeErrorHandlingTryDoCatchReadiness(summary);
  FinalizeErrorHandlingTryDoCatchReplayKey(summary);
}
