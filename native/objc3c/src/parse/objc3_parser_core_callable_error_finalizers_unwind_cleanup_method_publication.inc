  void PublishUnwindCleanupSiteCounters(
      Objc3MethodDecl &method,
      const Objc3UnwindCleanupProfile &profile) {
    method.unwind_cleanup_sites = profile.unwind_cleanup_sites;
    method.exceptional_exit_sites = profile.exceptional_exit_sites;
    method.cleanup_action_sites = profile.cleanup_action_sites;
    method.cleanup_scope_sites = profile.cleanup_scope_sites;
    method.cleanup_resume_sites = profile.cleanup_resume_sites;
    method.unwind_cleanup_normalized_sites = profile.normalized_sites;
    method.unwind_cleanup_fail_closed_sites = profile.fail_closed_sites;
    method.unwind_cleanup_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_unwind_cleanup_handoff =
        profile.deterministic_unwind_cleanup_handoff;
  }
