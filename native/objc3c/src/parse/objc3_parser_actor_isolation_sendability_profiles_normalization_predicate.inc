bool IsActorIsolationSendabilityProfileNormalized(
    std::size_t actor_isolation_sendability_sites,
    std::size_t actor_isolation_decl_sites,
    std::size_t actor_hop_sites,
    std::size_t sendable_annotation_sites,
    std::size_t non_sendable_crossing_sites,
    std::size_t isolation_boundary_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (actor_isolation_decl_sites > actor_isolation_sendability_sites ||
      actor_hop_sites > actor_isolation_sendability_sites ||
      sendable_annotation_sites > actor_isolation_sendability_sites ||
      non_sendable_crossing_sites > actor_isolation_sendability_sites ||
      isolation_boundary_sites > actor_isolation_sendability_sites ||
      normalized_sites > actor_isolation_sendability_sites ||
      gate_blocked_sites > actor_isolation_sendability_sites ||
      contract_violation_sites > actor_isolation_sendability_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites !=
      actor_isolation_sendability_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}
