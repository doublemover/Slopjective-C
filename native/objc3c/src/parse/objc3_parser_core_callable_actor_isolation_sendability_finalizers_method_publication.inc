  void PublishActorIsolationSendabilityProfile(
      Objc3MethodDecl &method,
      const Objc3ActorIsolationSendabilityProfile &profile) {
    method.actor_isolation_sendability_sites = profile.actor_isolation_sendability_sites;
    method.actor_isolation_decl_sites = profile.actor_isolation_decl_sites;
    method.actor_hop_sites = profile.actor_hop_sites;
    method.sendable_annotation_sites = profile.sendable_annotation_sites;
    method.non_sendable_crossing_sites = profile.non_sendable_crossing_sites;
    method.isolation_boundary_sites = profile.isolation_boundary_sites;
    method.actor_isolation_sendability_normalized_sites = profile.normalized_sites;
    method.actor_isolation_sendability_gate_blocked_sites = profile.gate_blocked_sites;
    method.actor_isolation_sendability_contract_violation_sites =
        profile.contract_violation_sites;
    method.deterministic_actor_isolation_sendability_handoff =
        profile.deterministic_actor_isolation_sendability_handoff;
    method.actor_isolation_sendability_profile = BuildActorIsolationSendabilityProfile(
        method.actor_isolation_sendability_sites,
        method.actor_isolation_decl_sites,
        method.actor_hop_sites,
        method.sendable_annotation_sites,
        method.non_sendable_crossing_sites,
        method.isolation_boundary_sites,
        method.actor_isolation_sendability_normalized_sites,
        method.actor_isolation_sendability_gate_blocked_sites,
        method.actor_isolation_sendability_contract_violation_sites,
        method.deterministic_actor_isolation_sendability_handoff);
    method.actor_isolation_sendability_profile_is_normalized =
        IsActorIsolationSendabilityProfileNormalized(
            method.actor_isolation_sendability_sites,
            method.actor_isolation_decl_sites,
            method.actor_hop_sites,
            method.sendable_annotation_sites,
            method.non_sendable_crossing_sites,
            method.isolation_boundary_sites,
            method.actor_isolation_sendability_normalized_sites,
            method.actor_isolation_sendability_gate_blocked_sites,
            method.actor_isolation_sendability_contract_violation_sites);
  }
