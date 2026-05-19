  void RebuildUnwindCleanupProfile(FunctionDecl &fn) {
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
  }

  void RebuildUnwindCleanupProfile(Objc3MethodDecl &method) {
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
  }
