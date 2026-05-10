static Objc3AwaitLoweringSuspensionStateSummary
BuildAwaitLoweringSuspensionStateSummaryFromProgramAst(
    const Objc3Program &ast) {
  Objc3AwaitLoweringSuspensionStateSummary summary;

  const auto checked_add = [&](std::size_t &total, std::size_t value) {
    if (value > std::numeric_limits<std::size_t>::max() - total) {
      total = std::numeric_limits<std::size_t>::max();
      summary.deterministic = false;
      return;
    }
    total += value;
  };

  const auto is_partitioned = [](std::size_t lhs, std::size_t rhs, std::size_t total) {
    return lhs <= std::numeric_limits<std::size_t>::max() - rhs && lhs + rhs == total;
  };

  const auto accumulate_await_profile = [&](const auto &declaration) {
    checked_add(summary.await_suspension_sites, declaration.await_suspension_sites);
    checked_add(summary.await_keyword_sites, declaration.await_keyword_sites);
    checked_add(summary.await_suspension_point_sites,
                declaration.await_suspension_point_sites);
    checked_add(summary.await_resume_sites, declaration.await_resume_sites);
    checked_add(summary.await_state_machine_sites,
                declaration.await_state_machine_sites);
    checked_add(summary.await_continuation_sites,
                declaration.await_continuation_sites);
    checked_add(summary.normalized_sites,
                declaration.await_suspension_normalized_sites);
    checked_add(summary.gate_blocked_sites,
                declaration.await_suspension_gate_blocked_sites);
    checked_add(summary.contract_violation_sites,
                declaration.await_suspension_contract_violation_sites);

    const bool profile_contract_normalized =
        declaration.await_keyword_sites <= declaration.await_suspension_sites &&
        declaration.await_suspension_point_sites <=
            declaration.await_suspension_sites &&
        declaration.await_resume_sites <= declaration.await_suspension_sites &&
        declaration.await_state_machine_sites <=
            declaration.await_suspension_point_sites &&
        declaration.await_continuation_sites <=
            declaration.await_suspension_point_sites &&
        declaration.await_suspension_normalized_sites <=
            declaration.await_suspension_sites &&
        declaration.await_suspension_gate_blocked_sites <=
            declaration.await_suspension_sites &&
        declaration.await_suspension_contract_violation_sites <=
            declaration.await_suspension_sites &&
        is_partitioned(declaration.await_suspension_normalized_sites,
                       declaration.await_suspension_gate_blocked_sites,
                       declaration.await_suspension_sites);
    summary.deterministic =
        summary.deterministic && declaration.await_suspension_profile_is_normalized &&
        declaration.deterministic_await_suspension_handoff &&
        profile_contract_normalized;
  };

  for (const auto &function_decl : ast.functions) {
    accumulate_await_profile(function_decl);
  }

  for (const auto &interface_decl : ast.interfaces) {
    for (const auto &method_decl : interface_decl.methods) {
      accumulate_await_profile(method_decl);
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      accumulate_await_profile(method_decl);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.await_keyword_sites <= summary.await_suspension_sites &&
      summary.await_suspension_point_sites <= summary.await_suspension_sites &&
      summary.await_resume_sites <= summary.await_suspension_sites &&
      summary.await_state_machine_sites <= summary.await_suspension_point_sites &&
      summary.await_continuation_sites <= summary.await_suspension_point_sites &&
      summary.normalized_sites <= summary.await_suspension_sites &&
      summary.gate_blocked_sites <= summary.await_suspension_sites &&
      summary.contract_violation_sites <= summary.await_suspension_sites &&
      is_partitioned(summary.normalized_sites,
                     summary.gate_blocked_sites,
                     summary.await_suspension_sites);
  return summary;
}
