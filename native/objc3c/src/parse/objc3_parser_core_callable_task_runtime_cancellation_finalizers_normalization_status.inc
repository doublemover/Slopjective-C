  void PublishTaskRuntimeCancellationNormalizationStatus(FunctionDecl &fn) {
    fn.task_runtime_cancellation_profile_is_normalized =
        IsTaskRuntimeCancellationProfileNormalized(
            fn.task_runtime_interop_sites,
            fn.runtime_hook_sites,
            fn.cancellation_check_sites,
            fn.cancellation_handler_sites,
            fn.suspension_point_sites,
            fn.cancellation_propagation_sites,
            fn.task_runtime_normalized_sites,
            fn.task_runtime_gate_blocked_sites,
            fn.task_runtime_contract_violation_sites);
  }

  void PublishTaskRuntimeCancellationNormalizationStatus(
      Objc3MethodDecl &method) {
    method.task_runtime_cancellation_profile_is_normalized =
        IsTaskRuntimeCancellationProfileNormalized(
            method.task_runtime_interop_sites,
            method.runtime_hook_sites,
            method.cancellation_check_sites,
            method.cancellation_handler_sites,
            method.suspension_point_sites,
            method.cancellation_propagation_sites,
            method.task_runtime_normalized_sites,
            method.task_runtime_gate_blocked_sites,
            method.task_runtime_contract_violation_sites);
  }
