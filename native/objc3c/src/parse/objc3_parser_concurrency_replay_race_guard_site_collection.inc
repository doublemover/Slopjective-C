bool IsConcurrencyReplaySymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("replay") != std::string::npos ||
         lowered.find("resume") != std::string::npos ||
         lowered.find("retry") != std::string::npos;
}

bool IsReplayProofSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("proof") != std::string::npos ||
         lowered.find("deterministic") != std::string::npos ||
         lowered.find("stable") != std::string::npos;
}

bool IsRaceGuardSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("race") != std::string::npos ||
         lowered.find("raceguard") != std::string::npos ||
         lowered.find("race_guard") != std::string::npos ||
         lowered.find("replayguard") != std::string::npos ||
         lowered.find("replay_guard") != std::string::npos ||
         lowered.find("lock") != std::string::npos;
}

bool IsTaskHandoffSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("handoff") != std::string::npos ||
         lowered.find("await") != std::string::npos ||
         lowered.find("task") != std::string::npos;
}

bool IsActorIsolationSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("actor") != std::string::npos ||
         lowered.find("isolation") != std::string::npos ||
         lowered.find("isolated") != std::string::npos;
}

struct Objc3ConcurrencyReplayRaceGuardSiteCounts {
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
};

void CollectConcurrencyReplayRaceGuardSitesFromSymbol(
    const std::string &symbol,
    Objc3ConcurrencyReplayRaceGuardSiteCounts &counts) {
  if (IsReplayProofSymbol(symbol)) {
    counts.replay_proof_sites += 1u;
  }
  if (IsRaceGuardSymbol(symbol)) {
    counts.race_guard_sites += 1u;
  }
  if (IsTaskHandoffSymbol(symbol)) {
    counts.task_handoff_sites += 1u;
  }
  if (IsActorIsolationSymbol(symbol)) {
    counts.actor_isolation_sites += 1u;
  }
  if (IsConcurrencyReplaySymbol(symbol) && counts.replay_proof_sites == 0u) {
    counts.replay_proof_sites += 1u;
  }
}

void CollectConcurrencyReplayRaceGuardProfileSymbol(
    const std::string &symbol,
    void *context) {
  auto *counts =
      static_cast<Objc3ConcurrencyReplayRaceGuardSiteCounts *>(context);
  CollectConcurrencyReplayRaceGuardSitesFromSymbol(symbol, *counts);
}

Objc3ConcurrencyReplayRaceGuardSiteCounts
CountConcurrencyReplayRaceGuardSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ConcurrencyReplayRaceGuardSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectConcurrencyReplayRaceGuardProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInBody(body, walker);
  return counts;
}
