Objc3AwaitSuspensionProfile BuildAwaitSuspensionProfileFromCounts(
    std::size_t await_keyword_sites,
    std::size_t await_suspension_point_sites,
    std::size_t await_resume_sites,
    std::size_t await_state_machine_sites,
    std::size_t await_continuation_sites) {
  Objc3AwaitSuspensionProfile profile;
  profile.await_keyword_sites = await_keyword_sites;
  profile.await_suspension_point_sites = await_suspension_point_sites;
  profile.await_resume_sites = await_resume_sites;
  profile.await_state_machine_sites = await_state_machine_sites;
  profile.await_continuation_sites = await_continuation_sites;
  profile.await_suspension_sites = profile.await_keyword_sites;
  if (profile.await_suspension_sites >
      std::numeric_limits<std::size_t>::max() -
          profile.await_suspension_point_sites) {
    profile.await_suspension_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.await_suspension_sites += profile.await_suspension_point_sites;
  }
  if (profile.await_suspension_sites >
      std::numeric_limits<std::size_t>::max() -
          profile.await_continuation_sites) {
    profile.await_suspension_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.await_suspension_sites += profile.await_continuation_sites;
  }
  profile.gate_blocked_sites = std::min(
      profile.await_suspension_sites,
      std::min(profile.await_resume_sites, profile.await_state_machine_sites));
  profile.normalized_sites =
      profile.await_suspension_sites - profile.gate_blocked_sites;
  if (profile.await_keyword_sites > profile.await_suspension_sites ||
      profile.await_suspension_point_sites > profile.await_suspension_sites ||
      profile.await_resume_sites > profile.await_suspension_sites ||
      profile.await_state_machine_sites > profile.await_suspension_sites ||
      profile.await_continuation_sites > profile.await_suspension_sites ||
      profile.normalized_sites > profile.await_suspension_sites ||
      profile.gate_blocked_sites > profile.await_suspension_sites ||
      profile.contract_violation_sites > profile.await_suspension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.await_suspension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.await_suspension_sites) {
    profile.contract_violation_sites = profile.await_suspension_sites;
  }
  profile.deterministic_await_suspension_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}
