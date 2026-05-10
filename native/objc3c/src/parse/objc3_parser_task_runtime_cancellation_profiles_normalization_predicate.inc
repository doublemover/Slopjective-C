bool IsTaskRuntimeCancellationProfileNormalized(
    std::size_t task_runtime_interop_sites,
    std::size_t runtime_hook_sites,
    std::size_t cancellation_check_sites,
    std::size_t cancellation_handler_sites,
    std::size_t suspension_point_sites,
    std::size_t cancellation_propagation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (runtime_hook_sites > task_runtime_interop_sites ||
      cancellation_check_sites > task_runtime_interop_sites ||
      cancellation_handler_sites > task_runtime_interop_sites ||
      suspension_point_sites > task_runtime_interop_sites ||
      cancellation_propagation_sites > cancellation_check_sites ||
      normalized_sites > task_runtime_interop_sites ||
      gate_blocked_sites > task_runtime_interop_sites ||
      contract_violation_sites > task_runtime_interop_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != task_runtime_interop_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}
