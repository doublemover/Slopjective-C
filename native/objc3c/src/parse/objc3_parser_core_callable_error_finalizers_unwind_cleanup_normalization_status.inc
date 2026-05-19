  void PublishUnwindCleanupNormalizationStatus(FunctionDecl &fn) {
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

  void PublishUnwindCleanupNormalizationStatus(Objc3MethodDecl &method) {
    method.unwind_cleanup_profile_is_normalized =
        IsUnwindCleanupProfileNormalized(
            method.unwind_cleanup_sites,
            method.exceptional_exit_sites,
            method.cleanup_action_sites,
            method.cleanup_scope_sites,
            method.cleanup_resume_sites,
            method.unwind_cleanup_normalized_sites,
            method.unwind_cleanup_fail_closed_sites,
            method.unwind_cleanup_contract_violation_sites);
  }
