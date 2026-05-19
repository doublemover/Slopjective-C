void CollectActorIsolationSendabilitySitesFromSymbol(
    const std::string &symbol,
    Objc3ActorIsolationSendabilitySiteCounts &counts) {
  if (IsActorIsolationDeclSymbol(symbol)) {
    counts.actor_isolation_decl_sites += 1u;
  }
  if (IsActorHopSymbol(symbol)) {
    counts.actor_hop_sites += 1u;
  }
  if (IsSendableAnnotationSymbol(symbol)) {
    counts.sendable_annotation_sites += 1u;
  }
  if (IsNonSendableCrossingSymbol(symbol)) {
    counts.non_sendable_crossing_sites += 1u;
  }
}

void CollectActorIsolationSendabilityProfileSymbol(
    const std::string &symbol,
    void *context) {
  auto *counts =
      static_cast<Objc3ActorIsolationSendabilitySiteCounts *>(context);
  CollectActorIsolationSendabilitySitesFromSymbol(symbol, *counts);
}
