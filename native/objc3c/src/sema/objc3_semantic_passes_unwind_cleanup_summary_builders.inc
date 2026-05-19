static Objc3UnwindCleanupSummary
BuildUnwindCleanupSummaryFromThrowsAndResultSummaries(
    const Objc3ThrowsPropagationSummary &throws_summary,
    const Objc3ResultLikeLoweringSummary &result_summary) {
  Objc3UnwindCleanupSummary summary;
  summary.unwind_cleanup_sites = throws_summary.throws_propagation_sites;

  const auto saturating_add = [&](std::size_t lhs, std::size_t rhs) {
    if (rhs > std::numeric_limits<std::size_t>::max() - lhs) {
      summary.deterministic = false;
      return std::numeric_limits<std::size_t>::max();
    }
    return lhs + rhs;
  };

  summary.exceptional_exit_sites = std::min(
      summary.unwind_cleanup_sites,
      saturating_add(throws_summary.namespace_segment_sites,
                     throws_summary.import_edge_candidate_sites));
  summary.cleanup_action_sites = std::min(
      summary.unwind_cleanup_sites,
      saturating_add(throws_summary.cache_invalidation_candidate_sites,
                     result_summary.result_payload_sites));
  summary.cleanup_scope_sites = std::min(summary.unwind_cleanup_sites,
                                         result_summary.branch_merge_sites);
  summary.cleanup_resume_sites = std::min(summary.unwind_cleanup_sites,
                                          result_summary.result_success_sites);
  summary.normalized_sites =
      std::min(summary.unwind_cleanup_sites, throws_summary.normalized_sites);
  summary.fail_closed_sites =
      summary.unwind_cleanup_sites - summary.normalized_sites;
  summary.contract_violation_sites = std::min(
      summary.unwind_cleanup_sites,
      saturating_add(throws_summary.contract_violation_sites,
                     result_summary.contract_violation_sites));
  summary.deterministic =
      summary.deterministic &&
      throws_summary.deterministic &&
      result_summary.deterministic &&
      summary.exceptional_exit_sites <= summary.unwind_cleanup_sites &&
      summary.cleanup_action_sites <= summary.unwind_cleanup_sites &&
      summary.cleanup_scope_sites <= summary.unwind_cleanup_sites &&
      summary.cleanup_resume_sites <= summary.unwind_cleanup_sites &&
      summary.normalized_sites <= summary.unwind_cleanup_sites &&
      summary.fail_closed_sites <= summary.unwind_cleanup_sites &&
      summary.contract_violation_sites <= summary.unwind_cleanup_sites &&
      summary.normalized_sites + summary.fail_closed_sites ==
          summary.unwind_cleanup_sites;
  return summary;
}
