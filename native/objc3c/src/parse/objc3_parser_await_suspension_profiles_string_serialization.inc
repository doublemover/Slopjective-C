std::string BuildAwaitSuspensionProfile(
    std::size_t await_suspension_sites,
    std::size_t await_keyword_sites,
    std::size_t await_suspension_point_sites,
    std::size_t await_resume_sites,
    std::size_t await_state_machine_sites,
    std::size_t await_continuation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_await_suspension_handoff) {
  std::ostringstream out;
  out << "await-suspension:await_suspension_sites="
      << await_suspension_sites
      << ";await_keyword_sites=" << await_keyword_sites
      << ";await_suspension_point_sites=" << await_suspension_point_sites
      << ";await_resume_sites=" << await_resume_sites
      << ";await_state_machine_sites=" << await_state_machine_sites
      << ";await_continuation_sites=" << await_continuation_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_await_suspension_handoff="
      << (deterministic_await_suspension_handoff ? "true" : "false");
  return out.str();
}
