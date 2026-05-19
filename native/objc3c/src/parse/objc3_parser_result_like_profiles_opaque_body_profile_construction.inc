Objc3ResultLikeProfile BuildResultLikeProfileFromOpaqueBody(bool has_body) {
  Objc3ResultLikeProfile profile;
  if (has_body) {
    profile.result_like_sites = 1u;
    profile.result_branch_sites = 1u;
    profile.branch_merge_sites = 1u;
  }
  profile.deterministic_result_like_lowering_handoff = true;
  return profile;
}
