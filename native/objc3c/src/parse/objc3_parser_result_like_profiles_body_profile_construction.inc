Objc3ResultLikeProfile BuildResultLikeProfileFromBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ResultLikeProfile profile;
  for (const auto &stmt : body) {
    CollectResultLikeStmtProfile(stmt.get(), profile);
  }

  if (profile.result_success_sites + profile.result_failure_sites !=
      profile.normalized_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.result_success_sites > profile.result_like_sites ||
      profile.result_failure_sites > profile.result_like_sites ||
      profile.result_branch_sites > profile.result_like_sites ||
      profile.result_payload_sites > profile.result_like_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.branch_merge_sites !=
      profile.result_like_sites) {
    profile.contract_violation_sites += 1u;
  }
  profile.deterministic_result_like_lowering_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}
