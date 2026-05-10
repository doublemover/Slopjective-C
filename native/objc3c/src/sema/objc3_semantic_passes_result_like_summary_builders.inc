static Objc3ResultLikeLoweringSummary BuildResultLikeLoweringSummaryFromProgramAst(
    const Objc3Program &ast) {
  Objc3ResultLikeLoweringSummary summary;

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

  const auto accumulate_result_like_profile = [&](const auto &declaration) {
    checked_add(summary.result_like_sites, declaration.result_like_sites);
    checked_add(summary.result_success_sites, declaration.result_success_sites);
    checked_add(summary.result_failure_sites, declaration.result_failure_sites);
    checked_add(summary.result_branch_sites, declaration.result_branch_sites);
    checked_add(summary.result_payload_sites, declaration.result_payload_sites);
    checked_add(summary.normalized_sites, declaration.result_normalized_sites);
    checked_add(summary.branch_merge_sites, declaration.result_branch_merge_sites);
    checked_add(summary.contract_violation_sites, declaration.result_contract_violation_sites);

    const bool profile_contract_normalized =
        declaration.result_success_sites <= declaration.result_like_sites &&
        declaration.result_failure_sites <= declaration.result_like_sites &&
        declaration.result_branch_sites <= declaration.result_like_sites &&
        declaration.result_payload_sites <= declaration.result_like_sites &&
        declaration.result_normalized_sites <= declaration.result_like_sites &&
        declaration.result_branch_merge_sites <= declaration.result_like_sites &&
        declaration.result_contract_violation_sites <= declaration.result_like_sites &&
        is_partitioned(declaration.result_success_sites,
                       declaration.result_failure_sites,
                       declaration.result_normalized_sites) &&
        is_partitioned(declaration.result_normalized_sites,
                       declaration.result_branch_merge_sites,
                       declaration.result_like_sites);
    summary.deterministic =
        summary.deterministic && declaration.result_like_profile_is_normalized &&
        declaration.deterministic_result_like_lowering_handoff &&
        profile_contract_normalized;
  };

  for (const auto &function_decl : ast.functions) {
    accumulate_result_like_profile(function_decl);
  }

  for (const auto &interface_decl : ast.interfaces) {
    for (const auto &method_decl : interface_decl.methods) {
      accumulate_result_like_profile(method_decl);
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      accumulate_result_like_profile(method_decl);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.result_success_sites <= summary.result_like_sites &&
      summary.result_failure_sites <= summary.result_like_sites &&
      summary.result_branch_sites <= summary.result_like_sites &&
      summary.result_payload_sites <= summary.result_like_sites &&
      summary.normalized_sites <= summary.result_like_sites &&
      summary.branch_merge_sites <= summary.result_like_sites &&
      summary.contract_violation_sites <= summary.result_like_sites &&
      is_partitioned(summary.result_success_sites,
                     summary.result_failure_sites,
                     summary.normalized_sites) &&
      is_partitioned(summary.normalized_sites,
                     summary.branch_merge_sites,
                     summary.result_like_sites);
  return summary;
}
