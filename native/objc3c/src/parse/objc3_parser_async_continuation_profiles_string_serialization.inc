std::string BuildAsyncContinuationProfile(
    std::size_t async_continuation_sites,
    std::size_t async_keyword_sites,
    std::size_t async_function_sites,
    std::size_t continuation_allocation_sites,
    std::size_t continuation_resume_sites,
    std::size_t continuation_suspend_sites,
    std::size_t async_state_machine_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_async_continuation_handoff) {
  std::ostringstream out;
  out << "async-continuation:async_continuation_sites="
      << async_continuation_sites
      << ";async_keyword_sites=" << async_keyword_sites
      << ";async_function_sites=" << async_function_sites
      << ";continuation_allocation_sites=" << continuation_allocation_sites
      << ";continuation_resume_sites=" << continuation_resume_sites
      << ";continuation_suspend_sites=" << continuation_suspend_sites
      << ";async_state_machine_sites=" << async_state_machine_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_async_continuation_handoff="
      << (deterministic_async_continuation_handoff ? "true" : "false");
  return out.str();
}
