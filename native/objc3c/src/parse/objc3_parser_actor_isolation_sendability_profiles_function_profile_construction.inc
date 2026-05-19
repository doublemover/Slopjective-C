Objc3ActorIsolationSendabilityProfile
BuildActorIsolationSendabilityProfileFromFunction(const FunctionDecl &fn) {
  const Objc3ActorIsolationSendabilitySiteCounts counts =
      CountActorIsolationSendabilitySitesInBody(fn.body);
  return BuildActorIsolationSendabilityProfileFromCounts(
      counts.actor_isolation_decl_sites,
      counts.actor_hop_sites,
      counts.sendable_annotation_sites,
      counts.non_sendable_crossing_sites);
}
