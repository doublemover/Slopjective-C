  void PublishAwaitSuspensionProfileFields(
      Objc3MethodDecl &method,
      const Objc3AwaitSuspensionProfile &profile) {
    method.await_suspension_sites = profile.await_suspension_sites;
    method.await_keyword_sites = profile.await_keyword_sites;
    method.await_suspension_point_sites = profile.await_suspension_point_sites;
    method.await_resume_sites = profile.await_resume_sites;
    method.await_state_machine_sites = profile.await_state_machine_sites;
    method.await_continuation_sites = profile.await_continuation_sites;
    method.await_suspension_normalized_sites = profile.normalized_sites;
    method.await_suspension_gate_blocked_sites = profile.gate_blocked_sites;
    method.await_suspension_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_await_suspension_handoff =
        profile.deterministic_await_suspension_handoff;
  }
