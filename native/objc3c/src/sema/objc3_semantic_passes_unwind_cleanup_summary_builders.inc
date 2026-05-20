static Objc3UnwindCleanupSummary BuildUnwindCleanupSummaryFromProgramAst(
    const Objc3Program &ast) {
  Objc3UnwindCleanupSummary summary;

  const auto checked_add = [&](std::size_t &total, std::size_t value) {
    if (value > std::numeric_limits<std::size_t>::max() - total) {
      total = std::numeric_limits<std::size_t>::max();
      summary.deterministic = false;
      return;
    }
    total += value;
  };

  const auto is_partitioned = [](std::size_t lhs, std::size_t rhs,
                                 std::size_t total) {
    return lhs <= std::numeric_limits<std::size_t>::max() - rhs &&
           lhs + rhs == total;
  };

  const auto accumulate_unwind_cleanup_profile = [&](const auto &declaration) {
    checked_add(summary.unwind_cleanup_sites,
                declaration.unwind_cleanup_sites);
    checked_add(summary.exceptional_exit_sites,
                declaration.exceptional_exit_sites);
    checked_add(summary.cleanup_action_sites,
                declaration.cleanup_action_sites);
    checked_add(summary.cleanup_scope_sites, declaration.cleanup_scope_sites);
    checked_add(summary.cleanup_resume_sites, declaration.cleanup_resume_sites);
    checked_add(summary.normalized_sites,
                declaration.unwind_cleanup_normalized_sites);
    checked_add(summary.fail_closed_sites,
                declaration.unwind_cleanup_fail_closed_sites);
    checked_add(summary.contract_violation_sites,
                declaration.unwind_cleanup_contract_violation_sites);

    const bool profile_contract_normalized =
        declaration.exceptional_exit_sites <=
            declaration.unwind_cleanup_sites &&
        declaration.cleanup_action_sites <= declaration.unwind_cleanup_sites &&
        declaration.cleanup_scope_sites <= declaration.unwind_cleanup_sites &&
        declaration.cleanup_resume_sites <= declaration.unwind_cleanup_sites &&
        declaration.unwind_cleanup_normalized_sites <=
            declaration.unwind_cleanup_sites &&
        declaration.unwind_cleanup_fail_closed_sites <=
            declaration.unwind_cleanup_sites &&
        declaration.unwind_cleanup_contract_violation_sites <=
            declaration.unwind_cleanup_sites &&
        is_partitioned(declaration.unwind_cleanup_normalized_sites,
                       declaration.unwind_cleanup_fail_closed_sites,
                       declaration.unwind_cleanup_sites);
    summary.deterministic =
        summary.deterministic &&
        declaration.unwind_cleanup_profile_is_normalized &&
        declaration.deterministic_unwind_cleanup_handoff &&
        profile_contract_normalized;
  };

  for (const auto &function_decl : ast.functions) {
    accumulate_unwind_cleanup_profile(function_decl);
  }

  for (const auto &interface_decl : ast.interfaces) {
    for (const auto &method_decl : interface_decl.methods) {
      accumulate_unwind_cleanup_profile(method_decl);
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      accumulate_unwind_cleanup_profile(method_decl);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.exceptional_exit_sites <= summary.unwind_cleanup_sites &&
      summary.cleanup_action_sites <= summary.unwind_cleanup_sites &&
      summary.cleanup_scope_sites <= summary.unwind_cleanup_sites &&
      summary.cleanup_resume_sites <= summary.unwind_cleanup_sites &&
      summary.normalized_sites <= summary.unwind_cleanup_sites &&
      summary.fail_closed_sites <= summary.unwind_cleanup_sites &&
      summary.contract_violation_sites <= summary.unwind_cleanup_sites &&
      is_partitioned(summary.normalized_sites, summary.fail_closed_sites,
                     summary.unwind_cleanup_sites);
  return summary;
}
