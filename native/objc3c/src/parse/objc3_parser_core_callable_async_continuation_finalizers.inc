  void FinalizeAsyncContinuationProfile(FunctionDecl &fn) {
    const Objc3AsyncContinuationProfile profile =
        BuildAsyncContinuationProfileFromFunction(fn);
    fn.async_continuation_sites = profile.async_continuation_sites;
    fn.async_keyword_sites = profile.async_keyword_sites;
    fn.async_function_sites = profile.async_function_sites;
    fn.continuation_allocation_sites = profile.continuation_allocation_sites;
    fn.continuation_resume_sites = profile.continuation_resume_sites;
    fn.continuation_suspend_sites = profile.continuation_suspend_sites;
    fn.async_state_machine_sites = profile.async_state_machine_sites;
    fn.async_continuation_normalized_sites = profile.normalized_sites;
    fn.async_continuation_gate_blocked_sites = profile.gate_blocked_sites;
    fn.async_continuation_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_async_continuation_handoff =
        profile.deterministic_async_continuation_handoff;
    fn.async_continuation_profile = BuildAsyncContinuationProfile(
        fn.async_continuation_sites,
        fn.async_keyword_sites,
        fn.async_function_sites,
        fn.continuation_allocation_sites,
        fn.continuation_resume_sites,
        fn.continuation_suspend_sites,
        fn.async_state_machine_sites,
        fn.async_continuation_normalized_sites,
        fn.async_continuation_gate_blocked_sites,
        fn.async_continuation_contract_violation_sites,
        fn.deterministic_async_continuation_handoff);
    fn.async_continuation_profile_is_normalized =
        IsAsyncContinuationProfileNormalized(
            fn.async_continuation_sites,
            fn.async_keyword_sites,
            fn.async_function_sites,
            fn.continuation_allocation_sites,
            fn.continuation_resume_sites,
            fn.continuation_suspend_sites,
            fn.async_state_machine_sites,
            fn.async_continuation_normalized_sites,
            fn.async_continuation_gate_blocked_sites,
            fn.async_continuation_contract_violation_sites);
  }

  void FinalizeAsyncContinuationProfile(Objc3MethodDecl &method) {
    const Objc3AsyncContinuationProfile profile =
        BuildAsyncContinuationProfileFromOpaqueBody(method);
    method.async_continuation_sites = profile.async_continuation_sites;
    method.async_keyword_sites = profile.async_keyword_sites;
    method.async_function_sites = profile.async_function_sites;
    method.continuation_allocation_sites = profile.continuation_allocation_sites;
    method.continuation_resume_sites = profile.continuation_resume_sites;
    method.continuation_suspend_sites = profile.continuation_suspend_sites;
    method.async_state_machine_sites = profile.async_state_machine_sites;
    method.async_continuation_normalized_sites = profile.normalized_sites;
    method.async_continuation_gate_blocked_sites = profile.gate_blocked_sites;
    method.async_continuation_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_async_continuation_handoff =
        profile.deterministic_async_continuation_handoff;
    method.async_continuation_profile = BuildAsyncContinuationProfile(
        method.async_continuation_sites,
        method.async_keyword_sites,
        method.async_function_sites,
        method.continuation_allocation_sites,
        method.continuation_resume_sites,
        method.continuation_suspend_sites,
        method.async_state_machine_sites,
        method.async_continuation_normalized_sites,
        method.async_continuation_gate_blocked_sites,
        method.async_continuation_contract_violation_sites,
        method.deterministic_async_continuation_handoff);
    method.async_continuation_profile_is_normalized =
        IsAsyncContinuationProfileNormalized(
            method.async_continuation_sites,
            method.async_keyword_sites,
            method.async_function_sites,
            method.continuation_allocation_sites,
            method.continuation_resume_sites,
            method.continuation_suspend_sites,
            method.async_state_machine_sites,
            method.async_continuation_normalized_sites,
            method.async_continuation_gate_blocked_sites,
            method.async_continuation_contract_violation_sites);
  }
