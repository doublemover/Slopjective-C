  void PublishAwaitSuspensionProfileFields(
      FunctionDecl &fn,
      const Objc3AwaitSuspensionProfile &profile) {
    fn.await_suspension_sites = profile.await_suspension_sites;
    fn.await_keyword_sites = profile.await_keyword_sites;
    fn.await_suspension_point_sites = profile.await_suspension_point_sites;
    fn.await_resume_sites = profile.await_resume_sites;
    fn.await_state_machine_sites = profile.await_state_machine_sites;
    fn.await_continuation_sites = profile.await_continuation_sites;
    fn.await_suspension_normalized_sites = profile.normalized_sites;
    fn.await_suspension_gate_blocked_sites = profile.gate_blocked_sites;
    fn.await_suspension_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_await_suspension_handoff =
        profile.deterministic_await_suspension_handoff;
  }
