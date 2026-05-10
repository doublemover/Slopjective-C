Objc3ActorIsolationSendabilityProfile
BuildActorIsolationSendabilityProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3ActorIsolationSendabilitySiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectActorIsolationSendabilityProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInOpaqueMethodBody(method, walker);
  return BuildActorIsolationSendabilityProfileFromCounts(
      counts.actor_isolation_decl_sites,
      counts.actor_hop_sites,
      counts.sendable_annotation_sites,
      counts.non_sendable_crossing_sites);
}
