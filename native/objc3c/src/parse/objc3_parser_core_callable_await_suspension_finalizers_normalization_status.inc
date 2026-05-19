  void PublishAwaitSuspensionNormalizationStatus(FunctionDecl &fn) {
    fn.await_suspension_profile_is_normalized =
        IsAwaitSuspensionProfileNormalized(
            fn.await_suspension_sites,
            fn.await_keyword_sites,
            fn.await_suspension_point_sites,
            fn.await_resume_sites,
            fn.await_state_machine_sites,
            fn.await_continuation_sites,
            fn.await_suspension_normalized_sites,
            fn.await_suspension_gate_blocked_sites,
            fn.await_suspension_contract_violation_sites);
  }

  void PublishAwaitSuspensionNormalizationStatus(Objc3MethodDecl &method) {
    method.await_suspension_profile_is_normalized =
        IsAwaitSuspensionProfileNormalized(
            method.await_suspension_sites,
            method.await_keyword_sites,
            method.await_suspension_point_sites,
            method.await_resume_sites,
            method.await_state_machine_sites,
            method.await_continuation_sites,
            method.await_suspension_normalized_sites,
            method.await_suspension_gate_blocked_sites,
            method.await_suspension_contract_violation_sites);
  }
