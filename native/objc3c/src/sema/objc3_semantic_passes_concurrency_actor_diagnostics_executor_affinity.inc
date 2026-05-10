static void DiagnoseConcurrencyExecutorAffinityCompletion(
    const Objc3ConcurrencyTaskCallableLegalityProfile &profile,
    bool async_declared, bool executor_affinity_declared,
    const std::string &executor_affinity_kind, int line, int column,
    std::vector<std::string> &diagnostics) {
  const std::size_t task_related_sites =
      profile.task_creation_sites + profile.task_group_scope_sites +
      profile.task_group_add_task_sites + profile.task_group_wait_next_sites +
      profile.task_group_cancel_all_sites + profile.cancellation_check_sites +
      profile.cancellation_handler_sites;
  if (!async_declared || task_related_sites == 0u) {
    return;
  }
  if (!executor_affinity_declared) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S231",
        "task semantics failed: async task callables require objc_executor affinity for deterministic executor hops"));
  }
  if (executor_affinity_declared && executor_affinity_kind == "main" &&
      profile.detached_task_creation_sites > 0u) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S232",
        "task semantics failed: detached task creation cannot use the main executor affinity"));
  }
}
