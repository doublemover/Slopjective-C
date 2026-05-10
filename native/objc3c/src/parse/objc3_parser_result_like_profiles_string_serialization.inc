std::string BuildResultLikeProfile(
    std::size_t result_like_sites,
    std::size_t result_success_sites,
    std::size_t result_failure_sites,
    std::size_t result_branch_sites,
    std::size_t result_payload_sites,
    std::size_t normalized_sites,
    std::size_t branch_merge_sites,
    std::size_t contract_violation_sites,
    bool deterministic_result_like_lowering_handoff) {
  std::ostringstream out;
  out << "result-like-lowering:result_like_sites=" << result_like_sites
      << ";result_success_sites=" << result_success_sites
      << ";result_failure_sites=" << result_failure_sites
      << ";result_branch_sites=" << result_branch_sites
      << ";result_payload_sites=" << result_payload_sites
      << ";normalized_sites=" << normalized_sites
      << ";branch_merge_sites=" << branch_merge_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_result_like_lowering_handoff="
      << (deterministic_result_like_lowering_handoff ? "true" : "false");
  return out.str();
}
