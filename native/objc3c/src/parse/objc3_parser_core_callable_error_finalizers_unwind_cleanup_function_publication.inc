  void PublishUnwindCleanupSiteCounters(
      FunctionDecl &fn,
      const Objc3UnwindCleanupProfile &profile) {
    fn.unwind_cleanup_sites = profile.unwind_cleanup_sites;
    fn.exceptional_exit_sites = profile.exceptional_exit_sites;
    fn.cleanup_action_sites = profile.cleanup_action_sites;
    fn.cleanup_scope_sites = profile.cleanup_scope_sites;
    fn.cleanup_resume_sites = profile.cleanup_resume_sites;
    fn.unwind_cleanup_normalized_sites = profile.normalized_sites;
    fn.unwind_cleanup_fail_closed_sites = profile.fail_closed_sites;
    fn.unwind_cleanup_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_unwind_cleanup_handoff =
        profile.deterministic_unwind_cleanup_handoff;
  }
