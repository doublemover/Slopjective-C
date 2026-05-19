Objc3ConcurrencyReplayRaceGuardProfile
BuildConcurrencyReplayRaceGuardProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3ConcurrencyReplayRaceGuardSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectConcurrencyReplayRaceGuardProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInOpaqueMethodBody(method, walker);
  return BuildConcurrencyReplayRaceGuardProfileFromCounts(
      counts.replay_proof_sites,
      counts.race_guard_sites,
      counts.task_handoff_sites,
      counts.actor_isolation_sites);
}
