Objc3ConcurrencyReplayRaceGuardProfile
BuildConcurrencyReplayRaceGuardProfileFromCounts(
    std::size_t replay_proof_sites,
    std::size_t race_guard_sites,
    std::size_t task_handoff_sites,
    std::size_t actor_isolation_sites) {
  Objc3ConcurrencyReplayRaceGuardProfile profile;
  profile.replay_proof_sites = replay_proof_sites;
  profile.race_guard_sites = race_guard_sites;
  profile.task_handoff_sites = task_handoff_sites;
  profile.actor_isolation_sites = actor_isolation_sites;
  profile.concurrency_replay_sites =
      std::max({profile.replay_proof_sites,
                profile.race_guard_sites,
                profile.task_handoff_sites,
                profile.actor_isolation_sites});
  profile.concurrency_replay_race_guard_sites =
      profile.concurrency_replay_sites;
  profile.guard_blocked_sites =
      std::min(profile.concurrency_replay_sites,
               profile.race_guard_sites / 2u);
  profile.deterministic_schedule_sites =
      profile.concurrency_replay_sites - profile.guard_blocked_sites;
  if (profile.concurrency_replay_race_guard_sites !=
          profile.concurrency_replay_sites ||
      profile.replay_proof_sites > profile.concurrency_replay_sites ||
      profile.race_guard_sites > profile.concurrency_replay_sites ||
      profile.task_handoff_sites > profile.concurrency_replay_sites ||
      profile.actor_isolation_sites > profile.concurrency_replay_sites ||
      profile.deterministic_schedule_sites >
          profile.concurrency_replay_sites ||
      profile.guard_blocked_sites > profile.concurrency_replay_sites ||
      profile.contract_violation_sites > profile.concurrency_replay_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.deterministic_schedule_sites + profile.guard_blocked_sites !=
      profile.concurrency_replay_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.concurrency_replay_sites) {
    profile.contract_violation_sites = profile.concurrency_replay_sites;
  }
  profile.deterministic_concurrency_replay_race_guard_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}
