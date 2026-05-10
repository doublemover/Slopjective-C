  void FinalizeUnwindCleanupProfile(FunctionDecl &fn) {
    const Objc3UnwindCleanupProfile profile = BuildUnwindCleanupProfileFromFunction(fn);
    fn.unwind_cleanup_sites = profile.unwind_cleanup_sites;
    fn.exceptional_exit_sites = profile.exceptional_exit_sites;
    fn.cleanup_action_sites = profile.cleanup_action_sites;
    fn.cleanup_scope_sites = profile.cleanup_scope_sites;
    fn.cleanup_resume_sites = profile.cleanup_resume_sites;
    fn.unwind_cleanup_normalized_sites = profile.normalized_sites;
    fn.unwind_cleanup_fail_closed_sites = profile.fail_closed_sites;
    fn.unwind_cleanup_contract_violation_sites = profile.contract_violation_sites;
    fn.deterministic_unwind_cleanup_handoff =
        profile.deterministic_unwind_cleanup_handoff;
    fn.unwind_cleanup_profile = BuildUnwindCleanupProfile(
        fn.unwind_cleanup_sites,
        fn.exceptional_exit_sites,
        fn.cleanup_action_sites,
        fn.cleanup_scope_sites,
        fn.cleanup_resume_sites,
        fn.unwind_cleanup_normalized_sites,
        fn.unwind_cleanup_fail_closed_sites,
        fn.unwind_cleanup_contract_violation_sites,
        fn.deterministic_unwind_cleanup_handoff);
    fn.unwind_cleanup_profile_is_normalized = IsUnwindCleanupProfileNormalized(
        fn.unwind_cleanup_sites,
        fn.exceptional_exit_sites,
        fn.cleanup_action_sites,
        fn.cleanup_scope_sites,
        fn.cleanup_resume_sites,
        fn.unwind_cleanup_normalized_sites,
        fn.unwind_cleanup_fail_closed_sites,
        fn.unwind_cleanup_contract_violation_sites);
  }

  void FinalizeUnwindCleanupProfile(Objc3MethodDecl &method) {
    const Objc3UnwindCleanupProfile profile = BuildUnwindCleanupProfileFromOpaqueBody(method);
    method.unwind_cleanup_sites = profile.unwind_cleanup_sites;
    method.exceptional_exit_sites = profile.exceptional_exit_sites;
    method.cleanup_action_sites = profile.cleanup_action_sites;
    method.cleanup_scope_sites = profile.cleanup_scope_sites;
    method.cleanup_resume_sites = profile.cleanup_resume_sites;
    method.unwind_cleanup_normalized_sites = profile.normalized_sites;
    method.unwind_cleanup_fail_closed_sites = profile.fail_closed_sites;
    method.unwind_cleanup_contract_violation_sites = profile.contract_violation_sites;
    method.deterministic_unwind_cleanup_handoff =
        profile.deterministic_unwind_cleanup_handoff;
    method.unwind_cleanup_profile = BuildUnwindCleanupProfile(
        method.unwind_cleanup_sites,
        method.exceptional_exit_sites,
        method.cleanup_action_sites,
        method.cleanup_scope_sites,
        method.cleanup_resume_sites,
        method.unwind_cleanup_normalized_sites,
        method.unwind_cleanup_fail_closed_sites,
        method.unwind_cleanup_contract_violation_sites,
        method.deterministic_unwind_cleanup_handoff);
    method.unwind_cleanup_profile_is_normalized = IsUnwindCleanupProfileNormalized(
        method.unwind_cleanup_sites,
        method.exceptional_exit_sites,
        method.cleanup_action_sites,
        method.cleanup_scope_sites,
        method.cleanup_resume_sites,
        method.unwind_cleanup_normalized_sites,
        method.unwind_cleanup_fail_closed_sites,
        method.unwind_cleanup_contract_violation_sites);
  }
