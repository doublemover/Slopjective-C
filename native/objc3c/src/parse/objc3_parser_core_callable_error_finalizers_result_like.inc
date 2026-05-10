  void FinalizeResultLikeProfile(FunctionDecl &fn) {
    const Objc3ResultLikeProfile profile = BuildResultLikeProfileFromBody(fn.body);
    fn.result_like_sites = profile.result_like_sites;
    fn.result_success_sites = profile.result_success_sites;
    fn.result_failure_sites = profile.result_failure_sites;
    fn.result_branch_sites = profile.result_branch_sites;
    fn.result_payload_sites = profile.result_payload_sites;
    fn.result_normalized_sites = profile.normalized_sites;
    fn.result_branch_merge_sites = profile.branch_merge_sites;
    fn.result_contract_violation_sites = profile.contract_violation_sites;
    fn.deterministic_result_like_lowering_handoff =
        profile.deterministic_result_like_lowering_handoff;
    fn.result_like_profile = BuildResultLikeProfile(
        fn.result_like_sites,
        fn.result_success_sites,
        fn.result_failure_sites,
        fn.result_branch_sites,
        fn.result_payload_sites,
        fn.result_normalized_sites,
        fn.result_branch_merge_sites,
        fn.result_contract_violation_sites,
        fn.deterministic_result_like_lowering_handoff);
    fn.result_like_profile_is_normalized = IsResultLikeProfileNormalized(
        fn.result_like_sites,
        fn.result_success_sites,
        fn.result_failure_sites,
        fn.result_branch_sites,
        fn.result_payload_sites,
        fn.result_normalized_sites,
        fn.result_branch_merge_sites,
        fn.result_contract_violation_sites);
  }

  void FinalizeResultLikeProfile(Objc3MethodDecl &method) {
    const Objc3ResultLikeProfile profile = BuildResultLikeProfileFromOpaqueBody(method.has_body);
    method.result_like_sites = profile.result_like_sites;
    method.result_success_sites = profile.result_success_sites;
    method.result_failure_sites = profile.result_failure_sites;
    method.result_branch_sites = profile.result_branch_sites;
    method.result_payload_sites = profile.result_payload_sites;
    method.result_normalized_sites = profile.normalized_sites;
    method.result_branch_merge_sites = profile.branch_merge_sites;
    method.result_contract_violation_sites = profile.contract_violation_sites;
    method.deterministic_result_like_lowering_handoff =
        profile.deterministic_result_like_lowering_handoff;
    method.result_like_profile = BuildResultLikeProfile(
        method.result_like_sites,
        method.result_success_sites,
        method.result_failure_sites,
        method.result_branch_sites,
        method.result_payload_sites,
        method.result_normalized_sites,
        method.result_branch_merge_sites,
        method.result_contract_violation_sites,
        method.deterministic_result_like_lowering_handoff);
    method.result_like_profile_is_normalized = IsResultLikeProfileNormalized(
        method.result_like_sites,
        method.result_success_sites,
        method.result_failure_sites,
        method.result_branch_sites,
        method.result_payload_sites,
        method.result_normalized_sites,
        method.result_branch_merge_sites,
        method.result_contract_violation_sites);
  }
