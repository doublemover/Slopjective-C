  void PublishActorIsolationSendabilityProfile(
      FunctionDecl &fn,
      const Objc3ActorIsolationSendabilityProfile &profile) {
    fn.actor_isolation_sendability_sites = profile.actor_isolation_sendability_sites;
    fn.actor_isolation_decl_sites = profile.actor_isolation_decl_sites;
    fn.actor_hop_sites = profile.actor_hop_sites;
    fn.sendable_annotation_sites = profile.sendable_annotation_sites;
    fn.non_sendable_crossing_sites = profile.non_sendable_crossing_sites;
    fn.isolation_boundary_sites = profile.isolation_boundary_sites;
    fn.actor_isolation_sendability_normalized_sites = profile.normalized_sites;
    fn.actor_isolation_sendability_gate_blocked_sites = profile.gate_blocked_sites;
    fn.actor_isolation_sendability_contract_violation_sites =
        profile.contract_violation_sites;
    fn.deterministic_actor_isolation_sendability_handoff =
        profile.deterministic_actor_isolation_sendability_handoff;
    fn.actor_isolation_sendability_profile = BuildActorIsolationSendabilityProfile(
        fn.actor_isolation_sendability_sites,
        fn.actor_isolation_decl_sites,
        fn.actor_hop_sites,
        fn.sendable_annotation_sites,
        fn.non_sendable_crossing_sites,
        fn.isolation_boundary_sites,
        fn.actor_isolation_sendability_normalized_sites,
        fn.actor_isolation_sendability_gate_blocked_sites,
        fn.actor_isolation_sendability_contract_violation_sites,
        fn.deterministic_actor_isolation_sendability_handoff);
    fn.actor_isolation_sendability_profile_is_normalized =
        IsActorIsolationSendabilityProfileNormalized(
            fn.actor_isolation_sendability_sites,
            fn.actor_isolation_decl_sites,
            fn.actor_hop_sites,
            fn.sendable_annotation_sites,
            fn.non_sendable_crossing_sites,
            fn.isolation_boundary_sites,
            fn.actor_isolation_sendability_normalized_sites,
            fn.actor_isolation_sendability_gate_blocked_sites,
            fn.actor_isolation_sendability_contract_violation_sites);
  }
