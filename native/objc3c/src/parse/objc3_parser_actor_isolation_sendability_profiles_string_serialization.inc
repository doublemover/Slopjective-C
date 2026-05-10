std::string BuildActorIsolationSendabilityProfile(
    std::size_t actor_isolation_sendability_sites,
    std::size_t actor_isolation_decl_sites,
    std::size_t actor_hop_sites,
    std::size_t sendable_annotation_sites,
    std::size_t non_sendable_crossing_sites,
    std::size_t isolation_boundary_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_actor_isolation_sendability_handoff) {
  std::ostringstream out;
  out << "actor-isolation-sendability:actor_isolation_sendability_sites="
      << actor_isolation_sendability_sites
      << ";actor_isolation_decl_sites=" << actor_isolation_decl_sites
      << ";actor_hop_sites=" << actor_hop_sites
      << ";sendable_annotation_sites=" << sendable_annotation_sites
      << ";non_sendable_crossing_sites=" << non_sendable_crossing_sites
      << ";isolation_boundary_sites=" << isolation_boundary_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_actor_isolation_sendability_handoff="
      << (deterministic_actor_isolation_sendability_handoff ? "true"
                                                            : "false");
  return out.str();
}
