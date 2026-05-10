  void FinalizeConcurrencyReplayRaceGuardProfile(FunctionDecl &fn) {
    const Objc3ConcurrencyReplayRaceGuardProfile profile =
        BuildConcurrencyReplayRaceGuardProfileFromFunction(fn);
    fn.concurrency_replay_race_guard_sites = profile.concurrency_replay_race_guard_sites;
    fn.concurrency_replay_sites = profile.concurrency_replay_sites;
    fn.replay_proof_sites = profile.replay_proof_sites;
    fn.race_guard_sites = profile.race_guard_sites;
    fn.task_handoff_sites = profile.task_handoff_sites;
    fn.actor_isolation_sites = profile.actor_isolation_sites;
    fn.deterministic_schedule_sites = profile.deterministic_schedule_sites;
    fn.concurrency_replay_guard_blocked_sites = profile.guard_blocked_sites;
    fn.concurrency_replay_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_concurrency_replay_race_guard_handoff =
        profile.deterministic_concurrency_replay_race_guard_handoff;
    fn.concurrency_replay_race_guard_profile =
        BuildConcurrencyReplayRaceGuardProfile(
            fn.concurrency_replay_race_guard_sites,
            fn.concurrency_replay_sites,
            fn.replay_proof_sites,
            fn.race_guard_sites,
            fn.task_handoff_sites,
            fn.actor_isolation_sites,
            fn.deterministic_schedule_sites,
            fn.concurrency_replay_guard_blocked_sites,
            fn.concurrency_replay_contract_violation_sites,
            fn.deterministic_concurrency_replay_race_guard_handoff);
    fn.concurrency_replay_race_guard_profile_is_normalized =
        IsConcurrencyReplayRaceGuardProfileNormalized(
            fn.concurrency_replay_race_guard_sites,
            fn.concurrency_replay_sites,
            fn.replay_proof_sites,
            fn.race_guard_sites,
            fn.task_handoff_sites,
            fn.actor_isolation_sites,
            fn.deterministic_schedule_sites,
            fn.concurrency_replay_guard_blocked_sites,
            fn.concurrency_replay_contract_violation_sites);
  }

  void FinalizeConcurrencyReplayRaceGuardProfile(Objc3MethodDecl &method) {
    const Objc3ConcurrencyReplayRaceGuardProfile profile =
        BuildConcurrencyReplayRaceGuardProfileFromOpaqueBody(method);
    method.concurrency_replay_race_guard_sites =
        profile.concurrency_replay_race_guard_sites;
    method.concurrency_replay_sites = profile.concurrency_replay_sites;
    method.replay_proof_sites = profile.replay_proof_sites;
    method.race_guard_sites = profile.race_guard_sites;
    method.task_handoff_sites = profile.task_handoff_sites;
    method.actor_isolation_sites = profile.actor_isolation_sites;
    method.deterministic_schedule_sites = profile.deterministic_schedule_sites;
    method.concurrency_replay_guard_blocked_sites = profile.guard_blocked_sites;
    method.concurrency_replay_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_concurrency_replay_race_guard_handoff =
        profile.deterministic_concurrency_replay_race_guard_handoff;
    method.concurrency_replay_race_guard_profile =
        BuildConcurrencyReplayRaceGuardProfile(
            method.concurrency_replay_race_guard_sites,
            method.concurrency_replay_sites,
            method.replay_proof_sites,
            method.race_guard_sites,
            method.task_handoff_sites,
            method.actor_isolation_sites,
            method.deterministic_schedule_sites,
            method.concurrency_replay_guard_blocked_sites,
            method.concurrency_replay_contract_violation_sites,
            method.deterministic_concurrency_replay_race_guard_handoff);
    method.concurrency_replay_race_guard_profile_is_normalized =
        IsConcurrencyReplayRaceGuardProfileNormalized(
            method.concurrency_replay_race_guard_sites,
            method.concurrency_replay_sites,
            method.replay_proof_sites,
            method.race_guard_sites,
            method.task_handoff_sites,
            method.actor_isolation_sites,
            method.deterministic_schedule_sites,
            method.concurrency_replay_guard_blocked_sites,
            method.concurrency_replay_contract_violation_sites);
  }
