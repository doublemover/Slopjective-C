Objc3ActorIsolationSendabilityProfile
BuildActorIsolationSendabilityProfileFromCounts(
    std::size_t actor_isolation_decl_sites,
    std::size_t actor_hop_sites,
    std::size_t sendable_annotation_sites,
    std::size_t non_sendable_crossing_sites) {
  Objc3ActorIsolationSendabilityProfile profile;
  profile.actor_isolation_decl_sites = actor_isolation_decl_sites;
  profile.actor_hop_sites = actor_hop_sites;
  profile.sendable_annotation_sites = sendable_annotation_sites;
  profile.non_sendable_crossing_sites = non_sendable_crossing_sites;
  profile.actor_isolation_sendability_sites =
      profile.actor_isolation_decl_sites;
  if (profile.actor_isolation_sendability_sites >
      std::numeric_limits<std::size_t>::max() - profile.actor_hop_sites) {
    profile.actor_isolation_sendability_sites =
        std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.actor_isolation_sendability_sites += profile.actor_hop_sites;
  }
  if (profile.actor_isolation_sendability_sites >
      std::numeric_limits<std::size_t>::max() -
          profile.sendable_annotation_sites) {
    profile.actor_isolation_sendability_sites =
        std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.actor_isolation_sendability_sites +=
        profile.sendable_annotation_sites;
  }
  profile.isolation_boundary_sites =
      std::min(profile.actor_isolation_decl_sites, profile.actor_hop_sites);
  profile.gate_blocked_sites = std::min(
      profile.actor_isolation_sendability_sites,
      profile.non_sendable_crossing_sites);
  profile.normalized_sites =
      profile.actor_isolation_sendability_sites - profile.gate_blocked_sites;
  if (profile.actor_isolation_decl_sites >
          profile.actor_isolation_sendability_sites ||
      profile.actor_hop_sites > profile.actor_isolation_sendability_sites ||
      profile.sendable_annotation_sites >
          profile.actor_isolation_sendability_sites ||
      profile.non_sendable_crossing_sites >
          profile.actor_isolation_sendability_sites ||
      profile.isolation_boundary_sites >
          profile.actor_isolation_sendability_sites ||
      profile.normalized_sites > profile.actor_isolation_sendability_sites ||
      profile.gate_blocked_sites >
          profile.actor_isolation_sendability_sites ||
      profile.contract_violation_sites >
          profile.actor_isolation_sendability_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.actor_isolation_sendability_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites >
      profile.actor_isolation_sendability_sites) {
    profile.contract_violation_sites =
        profile.actor_isolation_sendability_sites;
  }
  profile.deterministic_actor_isolation_sendability_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}
