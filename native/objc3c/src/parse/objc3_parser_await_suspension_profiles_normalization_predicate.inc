bool IsAwaitSuspensionProfileNormalized(
    std::size_t await_suspension_sites,
    std::size_t await_keyword_sites,
    std::size_t await_suspension_point_sites,
    std::size_t await_resume_sites,
    std::size_t await_state_machine_sites,
    std::size_t await_continuation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (await_keyword_sites > await_suspension_sites ||
      await_suspension_point_sites > await_suspension_sites ||
      await_resume_sites > await_suspension_sites ||
      await_state_machine_sites > await_suspension_sites ||
      await_continuation_sites > await_suspension_sites ||
      normalized_sites > await_suspension_sites ||
      gate_blocked_sites > await_suspension_sites ||
      contract_violation_sites > await_suspension_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != await_suspension_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}
