  void PublishTaskRuntimeCancellationSiteCounters(
      Objc3MethodDecl &method,
      const Objc3TaskRuntimeCancellationProfile &profile) {
    method.task_runtime_interop_sites = profile.task_runtime_interop_sites;
    method.runtime_hook_sites = profile.runtime_hook_sites;
    method.cancellation_check_sites = profile.cancellation_check_sites;
    method.cancellation_handler_sites = profile.cancellation_handler_sites;
    method.suspension_point_sites = profile.suspension_point_sites;
    method.cancellation_propagation_sites = profile.cancellation_propagation_sites;
    method.task_runtime_normalized_sites = profile.normalized_sites;
    method.task_runtime_gate_blocked_sites = profile.gate_blocked_sites;
    method.task_runtime_contract_violation_sites =
        profile.contract_violation_sites;
    method.task_runtime_cancellation_normalized_sites = profile.normalized_sites;
    method.task_runtime_cancellation_gate_blocked_sites =
        profile.gate_blocked_sites;
    method.task_runtime_cancellation_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_task_runtime_cancellation_handoff =
        profile.deterministic_task_runtime_cancellation_handoff;
  }
