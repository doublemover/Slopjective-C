Objc3ActorIsolationSendabilitySiteCounts
CountActorIsolationSendabilitySitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ActorIsolationSendabilitySiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectActorIsolationSendabilityProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInBody(body, walker);
  return counts;
}
