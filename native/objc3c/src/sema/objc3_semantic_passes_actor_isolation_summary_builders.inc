template <typename SiteMetadata>
static void AccumulateActorIsolationSendabilitySummaryFromSiteMetadata(
    const SiteMetadata &metadata,
    Objc3ActorIsolationSendabilitySummary &summary) {
  summary.actor_isolation_sendability_sites +=
      metadata.actor_isolation_sendability_sites;
  summary.actor_isolation_decl_sites += metadata.actor_isolation_decl_sites;
  summary.actor_hop_sites += metadata.actor_hop_sites;
  summary.sendable_annotation_sites += metadata.sendable_annotation_sites;
  summary.non_sendable_crossing_sites += metadata.non_sendable_crossing_sites;
  summary.isolation_boundary_sites += metadata.isolation_boundary_sites;
  summary.normalized_sites +=
      metadata.actor_isolation_sendability_normalized_sites;
  summary.gate_blocked_sites +=
      metadata.actor_isolation_sendability_gate_blocked_sites;
  summary.contract_violation_sites +=
      metadata.actor_isolation_sendability_contract_violation_sites;
  summary.deterministic =
      summary.deterministic &&
      metadata.deterministic_actor_isolation_sendability_handoff &&
      (metadata.actor_isolation_sendability_sites == 0u ||
       metadata.actor_isolation_sendability_profile_is_normalized);
}

static void FinalizeActorIsolationSendabilitySummaryDeterminism(
    Objc3ActorIsolationSendabilitySummary &summary) {
  summary.deterministic =
      summary.deterministic &&
      summary.actor_isolation_decl_sites <=
          summary.actor_isolation_sendability_sites &&
      summary.actor_hop_sites <= summary.actor_isolation_sendability_sites &&
      summary.sendable_annotation_sites <=
          summary.actor_isolation_sendability_sites &&
      summary.non_sendable_crossing_sites <=
          summary.actor_isolation_sendability_sites &&
      summary.isolation_boundary_sites <=
          summary.actor_isolation_sendability_sites &&
      summary.normalized_sites <= summary.actor_isolation_sendability_sites &&
      summary.gate_blocked_sites <= summary.actor_isolation_sendability_sites &&
      summary.gate_blocked_sites <= summary.non_sendable_crossing_sites &&
      summary.contract_violation_sites <=
          summary.actor_isolation_sendability_sites &&
      summary.normalized_sites + summary.gate_blocked_sites ==
          summary.actor_isolation_sendability_sites;
}

static Objc3ActorIsolationSendabilitySummary
BuildActorIsolationSendabilitySummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3ActorIsolationSendabilitySummary summary;
  for (const auto &entry : surface.functions) {
    AccumulateActorIsolationSendabilitySummaryFromSiteMetadata(entry.second,
                                                               summary);
  }
  for (const auto &interface_entry : surface.interfaces) {
    for (const auto &method_entry : interface_entry.second.methods) {
      AccumulateActorIsolationSendabilitySummaryFromSiteMetadata(
          method_entry.second, summary);
    }
  }
  for (const auto &implementation_entry : surface.implementations) {
    for (const auto &method_entry : implementation_entry.second.methods) {
      AccumulateActorIsolationSendabilitySummaryFromSiteMetadata(
          method_entry.second, summary);
    }
  }
  FinalizeActorIsolationSendabilitySummaryDeterminism(summary);
  return summary;
}

static Objc3ActorIsolationSendabilitySummary
BuildActorIsolationSendabilitySummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3ActorIsolationSendabilitySummary summary;
  for (const auto &metadata : handoff.functions_lexicographic) {
    AccumulateActorIsolationSendabilitySummaryFromSiteMetadata(metadata,
                                                               summary);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata :
         interface_metadata.methods_lexicographic) {
      AccumulateActorIsolationSendabilitySummaryFromSiteMetadata(
          method_metadata, summary);
    }
  }
  for (const auto &implementation_metadata :
       handoff.implementations_lexicographic) {
    for (const auto &method_metadata :
         implementation_metadata.methods_lexicographic) {
      AccumulateActorIsolationSendabilitySummaryFromSiteMetadata(
          method_metadata, summary);
    }
  }
  FinalizeActorIsolationSendabilitySummaryDeterminism(summary);
  return summary;
}
