Objc3AsyncContinuationProfile BuildAsyncContinuationProfileFromCounts(
    std::size_t async_keyword_sites,
    std::size_t async_function_sites,
    std::size_t continuation_allocation_sites,
    std::size_t continuation_resume_sites,
    std::size_t continuation_suspend_sites,
    std::size_t async_state_machine_sites) {
  Objc3AsyncContinuationProfile profile;
  profile.async_keyword_sites = async_keyword_sites;
  profile.async_function_sites = async_function_sites;
  profile.continuation_allocation_sites = continuation_allocation_sites;
  profile.continuation_resume_sites = continuation_resume_sites;
  profile.continuation_suspend_sites = continuation_suspend_sites;
  profile.async_state_machine_sites = async_state_machine_sites;
  profile.async_continuation_sites = profile.async_keyword_sites;
  if (profile.async_continuation_sites >
      std::numeric_limits<std::size_t>::max() -
          profile.async_function_sites) {
    profile.async_continuation_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.async_continuation_sites += profile.async_function_sites;
  }
  if (profile.async_continuation_sites >
      std::numeric_limits<std::size_t>::max() -
          profile.continuation_allocation_sites) {
    profile.async_continuation_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.async_continuation_sites +=
        profile.continuation_allocation_sites;
  }
  profile.gate_blocked_sites = std::min(
      profile.async_continuation_sites,
      std::min(profile.continuation_resume_sites,
               profile.continuation_suspend_sites));
  profile.normalized_sites =
      profile.async_continuation_sites - profile.gate_blocked_sites;
  if (profile.async_keyword_sites > profile.async_continuation_sites ||
      profile.async_function_sites > profile.async_continuation_sites ||
      profile.continuation_allocation_sites >
          profile.async_continuation_sites ||
      profile.continuation_resume_sites > profile.async_continuation_sites ||
      profile.continuation_suspend_sites > profile.async_continuation_sites ||
      profile.async_state_machine_sites > profile.async_continuation_sites ||
      profile.normalized_sites > profile.async_continuation_sites ||
      profile.gate_blocked_sites > profile.async_continuation_sites ||
      profile.contract_violation_sites > profile.async_continuation_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.async_continuation_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.async_continuation_sites) {
    profile.contract_violation_sites = profile.async_continuation_sites;
  }
  profile.deterministic_async_continuation_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}
