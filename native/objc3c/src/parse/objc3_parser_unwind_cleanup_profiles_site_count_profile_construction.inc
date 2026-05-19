Objc3UnwindCleanupProfile BuildUnwindCleanupProfileFromCounts(
    std::size_t exceptional_exit_sites,
    std::size_t cleanup_action_sites,
    std::size_t cleanup_scope_sites,
    std::size_t cleanup_resume_sites) {
  Objc3UnwindCleanupProfile profile;
  profile.exceptional_exit_sites = exceptional_exit_sites;
  profile.cleanup_action_sites = cleanup_action_sites;
  profile.cleanup_scope_sites = cleanup_scope_sites;
  profile.cleanup_resume_sites = cleanup_resume_sites;
  profile.unwind_cleanup_sites = profile.exceptional_exit_sites;
  if (profile.unwind_cleanup_sites >
      std::numeric_limits<std::size_t>::max() -
          profile.cleanup_action_sites) {
    profile.unwind_cleanup_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.unwind_cleanup_sites += profile.cleanup_action_sites;
  }
  if (profile.unwind_cleanup_sites >
      std::numeric_limits<std::size_t>::max() - profile.cleanup_scope_sites) {
    profile.unwind_cleanup_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.unwind_cleanup_sites += profile.cleanup_scope_sites;
  }
  profile.fail_closed_sites =
      std::min(profile.unwind_cleanup_sites, profile.cleanup_resume_sites);
  profile.normalized_sites =
      profile.unwind_cleanup_sites - profile.fail_closed_sites;
  if (profile.exceptional_exit_sites > profile.unwind_cleanup_sites ||
      profile.cleanup_action_sites > profile.unwind_cleanup_sites ||
      profile.cleanup_scope_sites > profile.unwind_cleanup_sites ||
      profile.cleanup_resume_sites > profile.unwind_cleanup_sites ||
      profile.normalized_sites > profile.unwind_cleanup_sites ||
      profile.fail_closed_sites > profile.unwind_cleanup_sites ||
      profile.contract_violation_sites > profile.unwind_cleanup_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.fail_closed_sites !=
      profile.unwind_cleanup_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.unwind_cleanup_sites) {
    profile.contract_violation_sites = profile.unwind_cleanup_sites;
  }
  profile.deterministic_unwind_cleanup_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}
