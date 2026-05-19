Objc3ConcurrencyReplayRaceGuardSiteCounts
CountConcurrencyReplayRaceGuardSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ConcurrencyReplayRaceGuardSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectConcurrencyReplayRaceGuardProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInBody(body, walker);
  return counts;
}
